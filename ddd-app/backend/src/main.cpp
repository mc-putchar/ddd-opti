#include "DroneState.hpp"
#include "SerialHandler.hpp"
#include "Path.hpp"

#include <thread>


int main() {

	std::cout << "Setting up serial connection..." << std::endl;
	SerialHandler handler;

	if (!handler.setup()) {
		std::cerr << "Failed to initialize serial connection." << std::endl;
		return 1;
	}

	// Create named pipes
	int fifoCmd = handler.createNamedPipe(PIPE_NAME_CMD_LINE);
	int fifoKey = handler.createNamedPipe(PIPE_NAME_KEY_HOOK);

	if (fifoCmd < 0 || fifoKey < 0) {
		return 1;
	}
	
	// Launch the transmitData function in a separate thread
	std::thread transmitThread([&handler, fifoCmd, fifoKey]() {
		handler.transmit(fifoCmd, fifoKey);
	});

	// Do other stuff in the main thread if needed
	std::cout << "Main thread doing other work..." << std::endl;
	Path path1("animation.json");

	// Join the thread before exiting (blocks until transmitData thread finishes)
	// Cleanup named pipes
	transmitThread.join();
	close(fifoCmd);
	close(fifoKey);
	unlink(PIPE_NAME_CMD_LINE);
	unlink(PIPE_NAME_KEY_HOOK);

	return 0;
}
