#include "DroneControl.hpp"
#include "DroneState.hpp"
#include "DronePath.hpp"
#include "Log.hpp"
#include "SerialHandler.hpp"
#include "WsServer.hpp"
// #include <fstream>
// #include <ios>

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
		// std::ofstream logfile("output.log", std::ios_base::app);
		// if (logfile.is_open())
		// {
		// 	log.set_output(&logfile, true);
		// 	INFO("Log", "Started logging to file.");
		// }
		// else
		// {
		// 	log.set_output(&std::cout, false);
		// 	ERROR("Log", "Failed to open log file");
		// }
		if (DEBUG_MODE)
		{
			log.set_verbosity(Log::DEBUG);
			log << Log::DEBUG << "Running in DEBUG mode." << std::endl;
		}
	}

	WsServer &wsServer = WsServer::getInstance();
	wsServer.start();

	SerialHandler &serialHandler = SerialHandler::getInstance();
	std::string const serialPort = getSerialPort();
	// if (serialPort.empty() || !serialHandler.setup())
	// 	return (1);
	std::thread transmitThread([&serialHandler]() { // Monitor if message are properly delivered to drones.
		serialHandler.monitorIncoming(); // Could come off a thread and added after each send if not setting too much latency.
	});

	DroneControl &control = DroneControl::getInstance();
	control.track(std::make_shared<DroneState>(0));
	control.track(std::make_shared<DroneState>(1));
	control.track(std::make_shared<DroneState>(2));
	control.track(std::make_shared<DroneState>(3));

	DronePath path("chocolate_Circle.json"); // Create path on the stack
	control.setPath(0, &path);
	DronePath path1("animation.json"); // Create path on the stack
	control.setPath(1, &path1);

	transmitThread.join();
	INFO("main", "exiting");
	return (0);
}
