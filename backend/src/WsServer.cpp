#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "json.hpp"
#include "ws.h"
#include "WsServer.hpp"

#define WS_PORT	4242
#define WS_HOST "0.0.0.0"

WsServer &WsServer::getInstance()
{
	static WsServer instance(WS_PORT, WS_HOST);
	return instance;
}

WsServer::WsServer(int port, char const *host) : server(), clients()
{
	this->server.host = host;
	this->server.port = static_cast<uint16_t>(port);
	this->server.thread_loop = 1;
	this->server.timeout_ms = 1000;
	this->server.evs.onopen = &(this->onopen);
	this->server.evs.onclose = &(this->onclose);
	this->server.evs.onmessage = &(this->onmessage);
	std::cout << "WebSocket Server listening on port: " << port << std::endl;
}

WsServer::~WsServer() {}


void WsServer::send(std::string const &msg, size_t client)
{
	if (client < this->clients.size())
		ws_sendframe_txt(this->clients[client], msg.c_str());
}

void WsServer::add_client(ws_cli_conn_t client)
{
	this->clients.push_back(client);
}

void WsServer::remove_client(ws_cli_conn_t client)
{
	auto it = std::find(this->clients.begin(), this->clients.end(), client);
	if (it != this->clients.end())
		this->clients.erase(it);
}


void WsServer::onopen(ws_cli_conn_t client)
{
	// (void)client;
	WsServer::getInstance().add_client(client);
	std::cout << "Client connected" << std::endl;
}

void WsServer::onclose(ws_cli_conn_t client)
{
	// (void)client;
	WsServer::getInstance().remove_client(client);
	std::cout << "Client disconnected" << std::endl;
}

using json = nlohmann::json;
void WsServer::onmessage(ws_cli_conn_t client, const unsigned char *msg, \
	uint64_t size, int type)
{
	(void)client;
	(void)type;
	if (size < 2) {
		return;
	}

	int const	index = msg[0] - '0';
	(void)index;
	try {
		std::string tmp(reinterpret_cast<char const *>(msg + 1), size - 1);
		json data = json::parse(tmp);
		if (data.contains("trim")) {
			// drones[index]->send(drones[index]->settrim(
			// 	data["trim"][0], data["trim"][1], data["trim"][2], data["trim"][3]));
		}

		if (data.contains("light")) {
			// drones[index]->send(drones[index]->setlight(data["light"][0], data["light"][1]));
		}

		if (data.contains("setpoint")) {
			// drones[index]->send(drones[index]->setpoint(data["setpoint"][0], data["setpoint"][1],
			// 											data["setpoint"][2], data["setpoint"][3]));
		}

		if (data.contains("armed")) {
			// if (data["armed"].get<bool>()) 
			// 	drones[index]->startup();
			// else
			// 	drones[index]->disarm();
		}

		if (data.contains("path")) {
			if (data["path"] == "play") {
				// Check if the unique pointer is not null
				// if (drones[index]->path != nullptr) {
				// 	drones[index]->path->sendFrameByFrame();
				// } else {
				// 	std::cerr << "Error: Path is null for drone " << index << std::endl;
				// }
			}
			if (data["path"] == "pause") {
				// drones[index]->path->paused.store(true);
			}
			if (data["path"] == "stop") {
				// drones[index]->path->sending=false;
			}
		}
		if (data.contains("frame")) {
			// if (drones[index]->path != nullptr) {
			// 	drones[index]->send(drones[index]->path->getCurrentFrame(data["frame"]).str().c_str());
			// } else {
			// 	std::cerr << "Error: Path is null for drone " << index << std::endl;
			// }
		}
	} catch (nlohmann::json::parse_error& e) {
		std::cerr << "Parse error: " << e.what() << std::endl;
	}
}
