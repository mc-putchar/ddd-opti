#pragma once

#include "crow_all.h"
#include <chrono>
#include <iostream>
#include <mutex>

class WsServer {

public:

	WsServer();
	~WsServer();

	void settingWsPort(char* arg);
	void settingWsConnection();
	crow::SimpleApp app;
	int	port;
	std::mutex wsMutex;
	crow::websocket::connection* wsConn = nullptr;

private:


};
