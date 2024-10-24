#pragma once

#include "crow_all.h"
#include "json.hpp"
#include "DroneState.hpp"
#include <chrono>
#include <iostream>
#include <mutex>

using json = nlohmann::json;

class DroneState;

class WsServer {

public:

	WsServer() = delete;
	WsServer(std::vector<DroneState> &ref);
	~WsServer();

	void settingWsPort(char* arg);
	void settingWsConnection();

	crow::SimpleApp app;
	int	port;
	std::mutex wsMutex;
	crow::websocket::connection* wsConn = nullptr;
	std::vector<DroneState> &drones;

private:


};
