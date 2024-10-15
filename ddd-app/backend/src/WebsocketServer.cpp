#include "crow_all.h"

int main() {
	crow::SimpleApp app;

	CROW_ROUTE(app, "/ws")
	.websocket(&app)  // Pass the app instance here
	.onopen([](crow::websocket::connection& conn) {
		(void)conn;
		std::cout << "Connection opened" << std::endl;
	})
	.onmessage([](crow::websocket::connection& conn, const std::string& message, bool is_binary) {
		(void)is_binary;
		std::cout << "Received message: " << message << std::endl;
		conn.send_text("Message received: " + message);  // Echo back the message
	})
	.onclose([](crow::websocket::connection& conn, const std::string& reason) {
		(void)conn;
		std::cout << "Connection closed: " << reason << std::endl;
	});

	app.port(18083).run();
}
