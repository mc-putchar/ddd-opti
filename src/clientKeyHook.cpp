#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

#include "DroneState.hpp"
#include "../libuiohook/include/uiohook.h"


#define VERSION 0.1
#define PIPE_NAME_KEY_HOOK "./tmp/ddd-data-interchange1"


void loop(int fd) {
  DroneState drone = DroneState();

  std::string key = "up";
  
   //key up
    drone.adjustpos("z", "+");
	//key down
	drone.adjustpos("z", "-");
	// key 
	drone.adjustpos("y", "+");
	drone.adjustpos("y", "-");

	drone.adjustpos("x", "+");
	drone.adjustpos("x", "-");

	drone.adjustpos("yaw", "+");
	drone.adjustpos("yaw", "-");

	//key a
	if (drone.is_armed()) 
        std::cout << "Drone already armed." << std::endl;
    if (!drone.startup(fd))
        std::cerr << "Failed to send startup sequence" << std::endl;

	//key d
	if (drone.send(fd, "0" + drone.disarm()) < 0)
	  std::cerr << "Failed to send" << std::endl;
	std::cout << "Drone disarmed" << std::endl;

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
