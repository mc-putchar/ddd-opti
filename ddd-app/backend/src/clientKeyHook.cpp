#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cmath>
#include <chrono>
#include <cstring>
#include <thread>
#include <termios.h>
#include "DroneState.hpp"
#include "Path.hpp"

#define VERSION 0.1
#define PIPE_NAME_KEY_HOOK "./tmp/ddd-data-interchange1"

// Function to configure terminal settings to read a single character
char getch() {
	char buf = 0;
	struct termios old;

	// Explicitly initialize all fields of the termios struct to zero
	memset(&old, 0, sizeof(old));  // Clear the struct

	if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
	old.c_lflag &= ~ICANON; // Disable canonical mode
	old.c_lflag &= ~ECHO;   // Disable echo
	if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr()");
	if (read(0, &buf, 1) < 0) return 0; // Read one character
	old.c_lflag |= ICANON; // Enable canonical mode
	old.c_lflag |= ECHO;   // Enable echo
	return (tcsetattr(0, TCSANOW, &old), buf); // Restore terminal settings
}

void loop(int fd) {
	DroneState drone = DroneState();

	while (true) {
		char key = getch(); // Read a single character
		std::cout << "Key pressed: " << key << std::endl;

		// Perform actions based on the key pressed
		switch (key) {
			case 's': // Move up
				if (drone.send(fd, drone.adjusttrim("z", "+")) < 0)
	   			  std::cerr << "Failed to send" << std::endl;
				std::cout << "Moving up" << std::endl;
				break;
			case 'w': // Move down
				if (drone.send(fd, drone.adjusttrim("z", "-")) < 0)
	   			  std::cerr << "Failed to send" << std::endl;
				std::cout << "Moving down" << std::endl;
				break;

			case 'k':
				if (drone.send(fd, drone.adjusttrim("x", "+")) < 0)
	   			  std::cerr << "Failed to send" << std::endl;
				std::cout << "Moving up" << std::endl;
				break;
			case 'i':
				if (drone.send(fd, drone.adjusttrim("x", "-")) < 0)
	   			  std::cerr << "Failed to send" << std::endl;
				std::cout << "Moving down" << std::endl;
				break;

			case 'j':
				if (drone.send(fd, drone.adjusttrim("y", "+")) < 0)
	   			  std::cerr << "Failed to send" << std::endl;
				std::cout << "Moving up" << std::endl;
				break;
			case 'l':
				if (drone.send(fd, drone.adjusttrim("y", "-")) < 0)
	   			  std::cerr << "Failed to send" << std::endl;
				std::cout << "Moving down" << std::endl;
				break;

			case 'a': // Move up
				if (drone.send(fd, drone.adjusttrim("yaw", "+")) < 0)
	   			  std::cerr << "Failed to send" << std::endl;
				std::cout << "Moving up" << std::endl;
				break;
			case 'd': // Move down
				if (drone.send(fd, drone.adjusttrim("yaw", "-")) < 0)
	   			  std::cerr << "Failed to send" << std::endl;
				std::cout << "Moving down" << std::endl;
				break;

			case 'b': 
				if (drone.send(fd, drone.adjustlight("servo", "-")) < 0)
				  std::cerr << "Failed to send" << std::endl;
				std::cout << "servo down" << std::endl;
				break;
			case 'n': 
				if (drone.send(fd, drone.adjustlight("servo", "+")) < 0)
				  std::cerr << "Failed to send" << std::endl;
				std::cout << "servo up" << std::endl;
				break;

			case 'r': // Move left
				if (drone.is_armed())
				  std::cout << "Drone already armed." << std::endl;
				if (!drone.startup(fd))
				  std::cerr << "Failed to send startup sequence" << std::endl;
				break;
			case 'x': 
				if (drone.send(fd, drone.disarm()) < 0)
				  std::cerr << "Failed to send" << std::endl;
				std::cout << "Drone disarmed" << std::endl;
				break;

			case 'q': // Quit the loop
				std::cout << "Exiting..." << std::endl;
				return; // Exit the loop

		}
	}
}

int main() {
	std::cout << "client KeyHook. Pres keys...\n" << std::endl;
	int const fifo = open(PIPE_NAME_KEY_HOOK, O_WRONLY);
	if (fifo < 0) {
		std::cerr << "Failed to open communication with controller.\n"
		<< "Ensure it is already running." << std::endl;
		return 1;
	}
	std::cout << "Pipe opened" << std::endl;


	loop(fifo); // Call the loop to start reading input and adjusting the drone

	close(fifo);
	return 0;
}
