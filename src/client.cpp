#include <iostream>

#include "DroneState.hpp"

#define VERSION 0.1
#define PROMPT "Input drone command: "

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
    } else if (cmd == "disarm" || cmd == "d") {
      if (drone.send(fd, "0" + drone.disarm()) < 0) {
        std::cerr << "Failed to send" << std::endl;
        continue;
      }
      std::cout << "Drone disarmed" << std::endl;
    } else if (cmd == "set" || cmd == "s") {
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
      if (drone.send(fd, "0" + drone.trim(x, y, z, yaw)) < 0) {
        std::cerr << "Failed to send" << std::endl;
        continue;
      }
      std::cout << "Set trim to: " << x << ", " << y << ", " << z << ", " << yaw
                << std::endl;
    } else {
      std::cout << "Command not recognized" << std::endl;
    }
  }
  std::cout << "Exiting program" << std::endl;
}

int main() {
  // TODO: Setup communication with controller
  loop(1);
  return (0);
}
