#include <csignal>
#include <iostream>
#include <thread>

#include "DroneControl.hpp"
#include "DroneState.hpp"
#include "NatNetClient.hpp"
#include "Path.hpp"
#include "SerialHandler.hpp"
#include "WsServer.hpp"

// TODO: 
// - Serial port Warn if disconnected (CRITICAL)
//	- Establish message queue for send/receive
// - Internal data model - DroneState
//	- track and update individual drones position, status, trajectory
//	- decouple from dependencies
// - Optitrack NatNet integration
//	- retrieve positional data of registered rigid bodies
//	- correlate with target DroneState and update its internal state
// - Unit Tests
// - Integration Tests
// - GUI
// - Documentation
volatile std::sig_atomic_t g_stopped;
void sighandler(int sig) {
	(void)sig;
	g_stopped = 1;
}

#define CONTAINER_IP	"172.18.0.2"
int main(void) {
	g_stopped = 0;
	::signal(SIGINT, sighandler);
	NatNetClient nnclient;
	s_ports ports(CMD_PORT, DATA_PORT);
	if (nnclient.init(&ports, CONTAINER_IP, DOCKER_HOST))
	{
		std::cerr << "Error: failed to initialize NatNet client." << std::endl;
	}
	std::thread NatNetThread([&nnclient]()->void {
		nnclient.listen_cmd();
	});

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
	NatNetThread.join();
	// wsServerThread.join();
	return 0;
}
