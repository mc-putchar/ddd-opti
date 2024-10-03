#include <fcntl.h>
#include <unistd.h>
#include <uiohook.h>

#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

#include "DroneState.hpp"
#include "../libuiohook/include/uiohook.h"


#define VERSION 0.1
#define PIPE_NAME_KEY_HOOK "./tmp/ddd-data-interchange1"

int fifo;

// Function to handle key events
void hook_event_dispatcher(uiohook_event *event) {
    DroneState drone = DroneState();

    if (event->type == EVENT_KEY_PRESSED) {
        std::cout << "Key pressed: " << event->data.keyboard.keycode << std::endl;

        // Check for specific key codes
        switch (event->data.keyboard.keycode) {
            case VC_W: // Up arrow key
				drone.send(fifo, "0" + drone.adjustpos("z", "+"));
                
                break;
            case VC_S: // Down arrow key
                drone.adjustpos("z", "-");
                break;
            // Add other keys if needed
        }
    }
	
	drone.adjustpos("y", "+");
	drone.adjustpos("y", "-");

	drone.adjustpos("x", "+");
	drone.adjustpos("x", "-");

	drone.adjustpos("yaw", "+");
	drone.adjustpos("yaw", "-");

	//key a
	if (drone.is_armed()) 
        std::cout << "Drone already armed." << std::endl;
    if (!drone.startup(fifo))
        std::cerr << "Failed to send startup sequence" << std::endl;

	//key d
	if (drone.send(fifo, "0" + drone.disarm()) < 0)
	  std::cerr << "Failed to send" << std::endl;
	std::cout << "Drone disarmed" << std::endl;

}

int main() {

  fifo = open(PIPE_NAME_KEY_HOOK, O_WRONLY);
  if (fifo < 0) {
    std::cerr << "Failed to open communication with controller. Ensure it is "
                 "already running."
              << std::endl;
    return (1);
  }
    // Set the hook event dispatcher
    hook_set_dispatch_proc(hook_event_dispatcher);

    // Run the hook to start capturing events
    if (hook_run() != UIOHOOK_SUCCESS) {
        std::cerr << "Failed to start hook!" << std::endl;
        return -1;
    }

    // Keep the program running to listen for events
    std::cout << "Press UP or DOWN arrow keys (press ESC to exit)..." << std::endl;
    while (true) {
        // Exit on ESC key press (key code 41)
        if (getchar() == 27) {
            break;
        }
    }
  close(fifo);

    // Cleanup and exit
    hook_stop();
    return 0;
}


