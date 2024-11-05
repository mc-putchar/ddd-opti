#include "DroneState.hpp"
#include "SerialHandler.hpp"
#include "Path.hpp"
#include "WsServer.hpp"

#include <thread>
#include <iostream>

int main(int argc, char ** argv) {

	std::vector<std::shared_ptr<DroneState>> drones;

	WsServer wsServer(drones);

	if (argc == 2)
		wsServer.settingWsPort(argv[1]);
	wsServer.settingWsConnection(); 
	std::thread wsServerThread([&wsServer]() { // Run the websocket connection in parallel
		wsServer.app.port(wsServer.port).multithreaded().run();
	});

	SerialHandler serialHandler(wsServer);
	
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

	int i = 0;
	while (true) {
		(void)i; // just to keep the backend alive.
	}

	transmitThread.join();
	wsServerThread.join();
	return 0;
}
