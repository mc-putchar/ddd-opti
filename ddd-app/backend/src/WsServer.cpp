#include "WsServer.hpp"


WsServer::WsServer() : port(8080) {}

WsServer::~WsServer() {}

void WsServer::settingWsPort(char* arg) {
	port = std::atoi(arg);
	std::cout << "WebSocket port = " << port << std::endl;
}


void WsServer::settingWsConnection() {

	CROW_ROUTE(app, "/ws")
	.websocket(&app)
	.onopen([this](crow::websocket::connection& conn) {
		(void)conn;
		std::cout << "WebSocket connection opened" << std::endl;
		// Store the connection in the shared pointer
		std::lock_guard<std::mutex> guard(wsMutex); // Lock to protect access
		std::cout << "wsconn ptr = " << wsConn << std::endl;
		wsConn = &conn; // Save the connection
	})
	.onmessage([this](crow::websocket::connection& conn, const std::string& message, bool is_binary) {
		(void)is_binary; // Ignore binary data for now
		(void)this;
		std::cout << "Received message: " << message << std::endl;

		// Echo the message back to the client
		// conn.send_text("8{\"front\":\"hello\")"); // on handshake
		conn.send_text(message);
	})
	.onclose([this](crow::websocket::connection& conn, const std::string& reason) {
		(void)conn;
		std::cout << "WebSocket connection closed: " << reason << std::endl;
		std::lock_guard<std::mutex> guard(wsMutex);
		wsConn= nullptr;
	});
}