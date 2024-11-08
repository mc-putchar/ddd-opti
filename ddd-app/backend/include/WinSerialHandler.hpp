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

std::string getSerialPort(void);

class SerialHandler {
public:
	SerialHandler(WsServer & ref, std::string const &port);
	~SerialHandler();

	bool	setup();
	void	monitorIncoming();
	int		send(std::string const &msg);
	void	parseTeleMsg(char *msg);
	int		sendFront(std::string const &msg);
	WsServer & wsServer;

private:
	HANDLE				hSerial;
	int					fifo;
	int					fifoKey;
	std::mutex			serialMutex;
	std::string const	&port;
};
