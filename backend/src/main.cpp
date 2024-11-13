#include <iostream>
#include <thread>

#include "DroneState.hpp"
#include "SerialHandler.hpp"
#include "Path.hpp"
#include "WsServer.hpp"

// TODO: 
// Refactor program control flow
// Segments:
// - Serial port connection and communication
//	- List available ports, connect to selected one
//	- Warn if disconnected (CRITICAL)
//	- Establish message queue for send/receive
// - WebSocket connection and communication
//	- crow/asio (check if we can decouple from this dependency?)
// - Internal data model - DroneState
//	- track and update individual drones position, status, trajectory
//	- decouple from dependencies
// - Optitrack MotiveAPI integration
//	- Initialize
//	- retrieve positional data of registered rigid bodies
//	- correlate with target DroneState and update its internal state
// - Unit Tests
// - Integration Tests
// - GUI
// - Documentation
#define WS_PORT	4242
int main(void) {
	std::string const serialPort = getSerialPort();
	if (serialPort.empty())
		return 1;

	std::vector<std::shared_ptr<DroneState>>	drones;

	WsServer &wsServer = WsServer::getInstance();

	SerialHandler serialHandler(serialPort);
	serialHandler.setup();
	std::thread transmitThread([&serialHandler]() { // Monitor if message are properly delivered to drones.
		serialHandler.monitorIncoming(); // Could come off a thread and added after each send if not setting too much latency.
	});

	drones.push_back(std::make_shared<DroneState>(0, serialHandler));
	drones.push_back(std::make_shared<DroneState>(1, serialHandler));
	drones.push_back(std::make_shared<DroneState>(2, serialHandler));
	drones.push_back(std::make_shared<DroneState>(3, serialHandler));

	std::cout << "drones array size  " << drones.size() << std::endl;

	Path path("Path01.json", *drones[0]); // Create path on the stack
	drones[0]->setPath(&path);

	Path path1("animation.json", *drones[1]); // Create path on the stack
	drones[1]->setPath(&path1);

	transmitThread.join();
	// wsServerThread.join();
	return 0;
}
