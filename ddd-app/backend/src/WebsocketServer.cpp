#include "crow_all.h"

int main() {
    crow::SimpleApp app;

    // Define a WebSocket route
    CROW_ROUTE(app, "/ws")
    .websocket()
    .onopen([](crow::websocket::connection& conn) {
        std::cout << "WebSocket connection opened!" << std::endl;
    })
    .onmessage([](crow::websocket::connection& conn, const std::string& message) {
        std::cout << "Received: " << message << std::endl;
        conn.send("Message received: " + message); // Echo back the message
    })
    .onclose([](crow::websocket::connection& conn) {
        std::cout << "WebSocket connection closed!" << std::endl;
    });

    app.port(8000).multithreaded().run(); // Start the server
}