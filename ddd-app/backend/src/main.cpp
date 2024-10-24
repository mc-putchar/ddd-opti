#include "DroneState.hpp"
#include "SerialHandler.hpp"
#include "Path.hpp"
#include "WsServer.hpp"

#include <thread>
#include <iostream>

int main(int argc, char ** argv) {

	std::vector<DroneState> drones;

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

	drones.emplace_back(0, serialHandler); //push back a drone instance with move semantic. the arg is the index;
	drones.emplace_back(1, serialHandler);
	drones.emplace_back(2, serialHandler);
	drones.emplace_back(3, serialHandler);

	std::cout << "drones array size  " << drones.size() << std::endl;

	auto path = std::make_unique<Path>("animation.json", drones[0]); // TODO might not need the whole move semantic anymore
	drones[0].setPath(std::move(path));

	std::this_thread::sleep_for(std::chrono::seconds(10));

	// drones[0].path->sendFrameByFrame();

	int i = 0;
	while (true) {
		(void)i; // just to keep the backend alive.
	}

	transmitThread.join();
	wsServerThread.join();
	return 0;
}
