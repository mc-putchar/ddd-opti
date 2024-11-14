#include <iostream>
#include <thread>

#include "DroneControl.hpp"
#include "DroneState.hpp"
#include "Path.hpp"
#include "SerialHandler.hpp"
#include "WsServer.hpp"

// TODO: 
// Refactor program control flow
// Segments:
// - Serial port connection and communication
//	- Warn if disconnected (CRITICAL)
//	- Establish message queue for send/receive
// - WebSocket connection and communication
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

	SerialHandler serialHandler(serialPort);
	serialHandler.setup();
	std::thread transmitThread([&serialHandler]() { // Monitor if message are properly delivered to drones.
		serialHandler.monitorIncoming(); // Could come off a thread and added after each send if not setting too much latency.
	});

	WsServer &wsServer = WsServer::getInstance();
	wsServer.start();

	DroneControl &control = DroneControl::getInstance();
	control.track(std::make_shared<DroneState>(0, serialHandler));
	control.track(std::make_shared<DroneState>(1, serialHandler));
	control.track(std::make_shared<DroneState>(2, serialHandler));
	control.track(std::make_shared<DroneState>(3, serialHandler));

	Path path("chocolate_Circle.json"); // Create path on the stack
	control.setPath(0, &path);

	Path path1("animation.json"); // Create path on the stack
	control.setPath(1, &path1);

	transmitThread.join();
	// wsServerThread.join();
	return 0;
}
