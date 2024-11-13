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

typedef struct __attribute__((packed)) s_tel_bat
{
  uint8_t id;
  uint8_t index;
  uint16_t Bat_volt;
  int16_t Bat_cur;
}t_tel_bat;

typedef struct __attribute__((packed)) s_tel_atitu
{
  uint8_t id;
  uint8_t index;
  float pitch;
  float roll;
  float yaw;
}t_tel_atitu;

typedef struct __attribute__((packed)) s_tel_rc
{
  uint8_t id;
  uint8_t index;
  uint16_t ch_1;
  uint16_t ch_2;
  uint16_t ch_3;
  uint16_t ch_4;
  uint16_t ch_5;
}t_tel_rc;

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
