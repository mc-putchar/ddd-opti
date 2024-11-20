#include "DataBroker.h"
#include "DroneControl.hpp"
#include "DroneState.hpp"
#include "DronePath.hpp"
#include "Log.hpp"
#include "NatNetClient.hpp"
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

#define CONTAINER_IP	"172.18.0.2"

#ifndef DEBUG_MODE
# define DEBUG_MODE	0
#endif

int main(void)
{
	/* Log */
	{
		using d3::log;
		log.set_output(&std::cout, false);
		if (DEBUG_MODE)
		{
			log.set_verbosity(Log::DEBUG);
			log << Log::DEBUG << "Running in DEBUG mode." << std::endl;
		}
	}

	struct s_databroker databro = {};
	if (broker_create(&databro))
		return (-1);

	WsServer &wsServer = WsServer::getInstance();
	wsServer.start();

	NatNetClient nnclient(databro.epoll_fd);
	s_ports ports(CMD_PORT, DATA_PORT);
	if (nnclient.connect(&ports, CONTAINER_IP, DOCKER_HOST, &databro))
	{
		ERROR("NatNet", "failed to initialize NatNet client.");
	}

	SerialHandler &serialHandler = SerialHandler::getInstance();
	std::string const serialPort = getSerialPort();
	if (serialPort.empty() || !serialHandler.setup())
		return (1);
	std::thread transmitThread([&serialHandler]() { // Monitor if message are properly delivered to drones.
		serialHandler.monitorIncoming(); // Could come off a thread and added after each send if not setting too much latency.
	});

	DroneControl &control = DroneControl::getInstance();
	control.track(std::make_shared<DroneState>(0, serialHandler));
	control.track(std::make_shared<DroneState>(1, serialHandler));
	control.track(std::make_shared<DroneState>(2, serialHandler));
	control.track(std::make_shared<DroneState>(3, serialHandler));

	DronePath path("chocolate_Circle.json"); // Create path on the stack
	control.setPath(0, &path);
	DronePath path1("animation.json"); // Create path on the stack
	control.setPath(1, &path1);

	broker_run(&databro);

	transmitThread.join();

	return (0);
}
