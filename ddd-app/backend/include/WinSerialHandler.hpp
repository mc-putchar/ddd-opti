#pragma once

#include <mutex>
#include <string>

#include "DroneState.hpp"
#include "WsServer.hpp"

#define BUFFER_SIZE 512
#define MIN_INTER_SEND 1.5
// #define SERIAL_PORT "COM3"
#define BAUD_RATE CBR_115200

class WsServer;

std::string getSerialPort(void);

class SerialHandler {
public:
	SerialHandler(WsServer & ref, std::string const &port);
	~SerialHandler();

	bool	setup();
	void	monitorIncoming();
	int		send(std::string const &msg);
	WsServer & wsServer;

private:
	HANDLE				hSerial;
	int					fifo;
	int					fifoKey;
	std::mutex			serialMutex;
	std::string const	&port;
};
