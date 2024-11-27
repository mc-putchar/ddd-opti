#pragma once

#include <mutex>
#include <string>

// #include "DataBroker.h"

#define BUFFER_SIZE 512
#define BAUD_RATE B115200
#ifdef __APPLE__
    #define SERIAL_PORT "/dev/cu.usbserial-0001"
#else
    #define SERIAL_PORT "/dev/ttyUSB0"
#endif


std::string getSerialPort();

class SerialHandler
{
public:
	static SerialHandler &getInstance();
	SerialHandler() = delete;
	SerialHandler(SerialHandler const &) = delete;
	SerialHandler(SerialHandler const &&) = delete;
	SerialHandler &operator=(SerialHandler const &) = delete;
	SerialHandler &operator=(SerialHandler const &&) = delete;
	~SerialHandler();

	bool setup();
	void monitorIncoming();
	int send(std::string const &msg);
	int sendFront(std::string const &msg);
	void parseTeleMsg(char *msg);

private:
	std::string const	port;
	int					port_fd;
	std::mutex			serialMutex;

	SerialHandler(std::string const &port);
	void send_to_ws(std::string msg);
	int setup_serial(void);
};
