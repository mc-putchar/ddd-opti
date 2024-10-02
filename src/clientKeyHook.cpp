#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

#include "DroneState.hpp"


#define VERSION 0.1
#define PIPE_NAME_KEY_HOOK "./tmp/ddd-data-interchange1"


void loop(int fd) {
  DroneState drone = DroneState();

  std::string key = "up";
  
    if (key == "up") {
      std::cout << "Position(x y z): ";
      std::string tmp;
      std::getline(std::cin, tmp);
      SetPoint point(tmp);
      if (drone.send(fd, "0" + drone.setpos(point.x, point.y, point.z, 0)) < 0) {
        std::cerr << "Failed to send" << std::endl;
      }
      std::cout << "Position update: " << point.x << ", " << point.y << ", "
                << point.z << std::endl;
    }
    
}

int main() {
  int const fifo = open(PIPE_NAME_KEY_HOOK, O_WRONLY);
  if (fifo < 0) {
    std::cerr << "Failed to open communication with controller. Ensure it is "
                 "already running."
              << std::endl;
    return (1);
  }
  loop(fifo);
  close(fifo);
  return (0);
}
