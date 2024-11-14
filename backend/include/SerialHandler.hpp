#pragma once

#ifdef _WIN32
# include "WinSerialHandler.hpp"
#else
#include <csignal>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <iostream>
#include <string>
#include <cstring>
#include <mutex>
#include "DroneState.hpp"
#include "WsServer.hpp"
#include "PacketDefinition.hpp"

#define BUFFER_SIZE 512
#ifdef __linux__
	#define SERIAL_PORT "/dev/ttyUSB0"
	#define BAUD_RATE B115200
#elif __APPLE__
	#define SERIAL_PORT "/dev/cu.usbserial-0001"
	#define BAUD_RATE B115200
#else
	#error "Unsupported platform"
#endif

class WsServer;

std::string getSerialPort();

class SerialHandler {
public:
	SerialHandler(std::string const &port);
	~SerialHandler();

	bool	setup();
	void	monitorIncoming();
	int		send(std::string const &msg);
	int		sendFront(std::string const &msg);
	void	parseTeleMsg(char *msg);

private:
	volatile std::sig_atomic_t	g_stopped;
	int				serial_port;
	int				fifo;
	int				fifoKey;
	std::string const		port;
	std::mutex			serialMutex;

	void send_to_ws(std::string msg);
	int setup_serial();

};
#endif
