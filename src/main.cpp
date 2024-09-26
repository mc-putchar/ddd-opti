#include <csignal>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>

#include "DroneState.hpp"
#include "Serial.hpp"

#define X_LIMIT 4
#define Y_LIMIT 2
#define Z_LIMIT 4
#define SAFETY_MARGIN 0.2

void TT_RigidBodyLocation(int rbIndex, float *x, float *y, float *z, float *qx,
                          float *qy, float *qz, float *qw, float *yaw,
                          float *pitch, float *roll);

#define BUFSIZE 4096
#define PROMPT "Input drone command: "
using DroneController::g_stopme;
using DroneController::sig_handler;
int loop(int port) {
  char buff[BUFSIZE];
  ssize_t r;
  std::string cmd;
  DroneState drone = DroneState();
  while (!g_stopme) {
    std::cout << PROMPT;
    std::getline(std::cin, cmd);
    if (cmd == "exit" || cmd == "quit" || cmd == "q") {
      sig_handler(42);
      break;
  } else if (cmd == "arm" || cmd == "a") {
      drone.arm();
      std::cout << "Drone armed" << std::endl;
    } else if (cmd == "disarm" || cmd == "d") {
      drone.disarm();
      std::cout << "Drone disarmed" << std::endl;
    } else if (cmd == "set" || cmd == "s") {
      std::cout << "Setpoint(x y z): ";
      std::string tmp;
      std::getline(std::cin, tmp);
      SetPoint point(tmp);
      drone.setpoint(point);
      std::cout << "New target: " << point.x << ", " << point.y << ", "
                << point.z << std::endl;
    } else if (cmd == "r") {
      if ((r = read(port, buff, BUFSIZE - 1)) > 0) {
        buff[r] = 0;
        std::cout << "serial: " << buff << std::endl;
      }
    } else if (cmd == "c" || cmd == "check") {
      std::cout << "Drone state: " << drone.get_state() << std::endl;
    } else if (cmd == "t" || cmd == "trim") {
      std::cout << "Trim(x y z yaw): ";
      std::string tmp;
      std::getline(std::cin, tmp);
      float x, y, z, yaw;

      std::istringstream iss(tmp);
      iss >> x;
      iss >> y;
      iss >> z;
      iss >> yaw;
      drone.trim(x, y, z, yaw);
      std::cout << "Set trim to: " << x << ", " << y << ", " << z << ", " << yaw
                << std::endl;
    } else {
      std::cout << "Command not recognized" << std::endl;
    }
    if ((r = read(port, buff, BUFSIZE - 1)) > 0) {
      buff[r] = 0;
      std::cout << "serial: " << buff << std::endl;
    }
  }
  std::cout << "Exiting program" << std::endl;
  return 0;
}

int main() {
  float x, y, z;
  float qx, qy, qz, qw;
  float yaw, pitch, roll;
  std::vector<DroneState> drones;
  DroneState drone = DroneState(0);
  drones.push_back(drone);
  Serial serial = Serial();

  if (!serial.setup()) {
    std::cerr << "Failed to establish serial connection to transmitter"
              << std::endl;
    return (1);
  }
  // std::signal(SIGINT, &sig_handler);
  // std::signal(SIGTERM, &sig_handler);
  TT_RigidBodyLocation(0, &x, &y, &z, &qx, &qy, &qz, &qw, &yaw, &pitch, &roll);
  std::cout << "x: " << x << " y: " << y << " z: " << z << std::endl
            << "qx: " << qx << " qy: " << qy << " qz: " << qz << " qw: " << qw
            << std::endl
            << "yaw: " << yaw << " pitch: " << pitch << " roll: " << roll
            << std::endl;
  std::cout << "Drone state: " << drone.get_state() << std::endl;
  std::thread main_loop(loop, serial.get_fd());
  std::thread transmitter(&Serial::handle_serial, &serial, std::ref(drones));
  main_loop.join();
  transmitter.join();
  return (0);
}
