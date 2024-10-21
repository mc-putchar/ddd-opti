#include "DroneState.hpp"
#include "SerialHandler.hpp"
#include "Path.hpp"
#include "crow_all.h"

#include <thread>
#include <iostream>
#include <memory>
#include <mutex>

// Shared pointer for WebSocket connection
crow::websocket::connection* wsConn = nullptr;
std::mutex wsMutex; // Mutex to protect the WebSocket connection
std::mutex serialMutex; // to protect the write to serial; 

int settingWsPort(int argc, char ** argv) {
	int wsport = 8080;
	if (argc == 2)
		wsport = std::atoi(argv[1]);
	std::cout << "port = " << wsport << std::endl;
	return wsport;
}

crow::SimpleApp & settingWsConnection() {
	static crow::SimpleApp app;

	CROW_ROUTE(app, "/ws")
	.websocket(&app)
	.onopen([](crow::websocket::connection& conn) {
		(void)conn;
		std::cout << "WebSocket connection opened" << std::endl;
		// Store the connection in the shared pointer
		std::lock_guard<std::mutex> guard(wsMutex); // Lock to protect access
		std::cout << "wsconn ptr = " << wsConn << std::endl;
		wsConn = &conn; // Save the connection
	})
	.onmessage([](crow::websocket::connection& conn, const std::string& message, bool is_binary) {
		(void)is_binary; // Ignore binary data for now
		std::cout << "Received message: " << message << std::endl;

		// Echo the message back to the client
		// conn.send_text("8{\"front\":\"hello\")"); // on handshake
		conn.send_text(message);
	})
	.onclose([](crow::websocket::connection& conn, const std::string& reason) {
		(void)conn;
		std::cout << "WebSocket connection closed: " << reason << std::endl;
		std::lock_guard<std::mutex> guard(wsMutex);
		wsConn= nullptr;
	});
	return app;
}

int main(int argc, char ** argv) {

	const int wsport = settingWsPort(argc, argv);


	SerialHandler serialHandler(wsMutex, wsConn);
	serialHandler.setup();
	const int fifoCmd = serialHandler.createNamedPipe(PIPE_NAME_CMD_LINE);
	const int fifoKey = serialHandler.createNamedPipe(PIPE_NAME_KEY_HOOK);
	const int serial_port = serialHandler.getSerialPort();
	if (fifoCmd < 0 || fifoKey < 0) return 1;
	std::thread transmitThread([&serialHandler, fifoCmd, fifoKey]() { // Launch transmitData fnct in thread
		serialHandler.transmit(fifoCmd, fifoKey);
	});

	std::vector<DroneState> drones;
	drones.emplace_back(0, serial_port, serialMutex); //push back a drone instance with move semantic. the arg is the index;
	drones.emplace_back(1, serial_port, serialMutex);
	drones.emplace_back(2, serial_port, serialMutex);
	drones.emplace_back(3, serial_port, serialMutex);

	crow::SimpleApp & app = settingWsConnection(); // WebSocket server with Crow
	std::thread crowThread([&app, wsport]() { // Run the Crow app in another thread so it doesn't block the main thread
		app.port(wsport).multithreaded().run();
	});


	// Do other stuff in the main thread if needed
	std::cout << "Main thread doing other work..." << std::endl;

	auto path = std::make_unique<Path>("animation.json", drones[0]); // Create the unique_ptr directly
	drones[0].setPath(std::move(path));



	std::this_thread::sleep_for(std::chrono::seconds(10));

	drones[0].path->sendFrameByFrame(wsConn, wsMutex);


	int i = 0;
	while (true) { // test to see if the connectio is working
		(void)i;
		//  std::this_thread::sleep_for(std::chrono::seconds(1));
		// std::lock_guard<std::mutex> guard(wsMutex);
		// if (wsConn) { // If the connection exists
		//     std::cout << "Sending message from main thread..." << std::endl;
		// 	std::stringstream ss;
		// 	ss << "Data " << i << " from main thread";
		//     wsConn->send_text(ss.str());
		// 	i++;
		// } else {
		//     std::cout << "No active WebSocket connection" << std::endl;
		// }
	}



	transmitThread.join();
	crowThread.join();
	close(fifoCmd);
	close(fifoKey);
	unlink(PIPE_NAME_CMD_LINE);
	unlink(PIPE_NAME_KEY_HOOK);

	return 0;
}
