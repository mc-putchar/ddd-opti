#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

#include "DroneState.hpp"

#define TIME_TO_SETPOINT 1.8 // Setpoint oscillation period in seconds

#define VERSION 0.1
#define PIPE_NAME "/tmp/ddd-data-interchange"
#define PROMPT "Input drone command: "

void droneFloat(DroneState & drone, int fd, int target_oscillations) {
    float Sx = 0.0f, Sy = 0.0f, Sz = 1.0f; // Setpoints
    float x = 0.0f, y = 0.0f, z = 0.0f; // Current values

    const float frequency = 2 * M_PI / TIME_TO_SETPOINT;
    int oscillationCount = 0; // Initialize oscillation counter

    // Track the start time
    auto startTime = std::chrono::steady_clock::now();

    // Condition for the while loop
    while (oscillationCount < target_oscillations) {

        // Update z to oscillate between 0 and 1
        auto currentTime = std::chrono::steady_clock::now();
        float elapsedSeconds = std::chrono::duration<float>(currentTime - startTime).count();

        // Calculate z using sine wave oscillation
        z = (std::sin(frequency * elapsedSeconds) + 1) / 2;

        // Check for oscillation completion
        if (elapsedSeconds >= TIME_TO_SETPOINT) {
            oscillationCount++; // Increment counter
            startTime = std::chrono::steady_clock::now(); // Reset start time
        }
        if (drone.send(fd, std::string("0") +"{'setpoint':[" + 
          std::to_string(Sx) + "," + std::to_string(Sy) + "," + std::to_string(Sz) + "]}") < 0) {
          std::cerr << "Failed to send" << std::endl;
          continue;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (drone.send(fd, std::string("0") + "{'pos':[" + 
          std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "]}") < 0) {
          std::cerr << "Failed to send" << std::endl;
          continue;
        }

        // Wait for 50 milliseconds
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    std::cout << "Completed " << target_oscillations << " oscillations." << std::endl;
}

void loop(int fd) {
  std::string cmd;
  DroneState drone = DroneState();
  while (1) {
    std::cout << PROMPT;
    std::getline(std::cin, cmd);
    if (cmd == "exit" || cmd == "quit" || cmd == "q")
      break;
    if (cmd == "arm" || cmd == "a") {
      if (drone.is_armed()) {
        std::cout << "Drone already armed." << std::endl;
        continue;
      }
      if (!drone.startup(fd)) {
        std::cerr << "Failed to send startup sequence" << std::endl;
        continue;
      }
      std::cout << "Drone armed" << std::endl;
    }
    else if (cmd == "disarm" || cmd == "d") {
      if (drone.send(fd, "0" + drone.disarm()) < 0) {
        std::cerr << "Failed to send" << std::endl;
        continue;
      }
      std::cout << "Drone disarmed" << std::endl;
    }
    else if (cmd == "set" || cmd == "s") {
      std::cout << "Setpoint(x y z): ";
      std::string tmp;
      std::getline(std::cin, tmp);
      SetPoint point(tmp);
      if (drone.send(fd, "0" + drone.setpoint(point)) < 0) {
        std::cerr << "Failed to send" << std::endl;
        continue;
      }
      std::cout << "New target: " << point.x << ", " << point.y << ", "
                << point.z << std::endl;
    }
    else if (cmd == "pos" || cmd == "p") {
      std::cout << "Position(x y z): ";
      std::string tmp;
      std::getline(std::cin, tmp);
      SetPoint point(tmp);
      if (drone.send(fd, "0" + drone.setpos(point.x, point.y, point.z, 0)) < 0) {
        std::cerr << "Failed to send" << std::endl;
        continue;
      }
      std::cout << "Position update: " << point.x << ", " << point.y << ", "
                << point.z << std::endl;
    }
    else if (cmd == "c" || cmd == "check") {
      std::cout << "Drone state: " << drone.get_state() << std::endl;
    }
    else if (cmd == "t" || cmd == "trim") {
      std::cout << "Trim(x y z yaw): ";
      std::string tmp;
      std::getline(std::cin, tmp);
      float x, y, z, yaw;

      std::istringstream iss(tmp);
      iss >> x;
      iss >> y;
      iss >> z;
      iss >> yaw;
      if (drone.send(fd, "0" + drone.trim(x, y, z, yaw)) < 0) {
        std::cerr << "Failed to send" << std::endl;
        continue;
      }
      std::cout << "Set trim to: " << x << ", " << y << ", " << z << ", " << yaw
                << std::endl;
    }
    else if (cmd == "f" || cmd == "float") {
        // make the drone go up and down floating.
      droneFloat(drone, fd, 3);
    }
    else {
      std::cout << "Command not recognized" << std::endl;
    }
  }
  std::cout << "Exiting program" << std::endl;
}

int main() {
  int const fifo = open(PIPE_NAME, O_WRONLY);
  if (fifo < 0) {
    std::cerr << "Failed to open communication with controller. Ensure it is "
                 "already running."
              << std::endl;
    // return (1);
  }
  loop(fifo);
  close(fifo);
  return (0);
}
