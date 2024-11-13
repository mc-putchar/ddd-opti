#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "ws.h"

class WsServer {
	public:
		static WsServer &getInstance();
		~WsServer();
		WsServer(WsServer const &) = delete;
		WsServer(WsServer const &&) = delete;
		void operator=(WsServer const &) = delete;
		void operator=(WsServer const &&) = delete;

		void send(std::string const &msg, size_t client = 0);
		void add_client(ws_cli_conn_t client);
		void remove_client(ws_cli_conn_t client);

	private:
		WsServer(int port, char const *host = "0.0.0.0");
		struct ws_server server;
		std::vector<ws_cli_conn_t> clients;

		static void onopen(ws_cli_conn_t client);
		static void onclose(ws_cli_conn_t client);
		static void onmessage(ws_cli_conn_t client, const unsigned char *msg, \
			uint64_t size, int type);
};
