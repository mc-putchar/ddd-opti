#include <winsock2.h>
#include <Windows.h>
#include <WinBase.h>

#include <csignal>
#include <iostream>
#include <map>
#include <string>

#include "WinSerialHandler.hpp"

namespace ddd {
	volatile std::sig_atomic_t g_stop(0);
	void sighandler(int sig) {
		(void)sig;
		g_stop = 1;
	}
}

#define DEVICE_MAX_PATH	5000
static std::map<int, std::string> listSerialDevices()
{
	std::map<int, std::string> portMap;
	char lpTargetPath[DEVICE_MAX_PATH];
	std::string const str = "COM";
	std::cout << "Connected serial ports:" << std::endl;
	int idx(0);
	for (int i = 0; i < 255; i++) {
		std::string tmp = str + std::to_string(i);
		DWORD res = ::QueryDosDevice(tmp.c_str(), lpTargetPath, DEVICE_MAX_PATH);
		if (res) {
			portMap[++idx] = tmp;
			std::cout << "[" << idx << "]: " << tmp << std::endl;
		}
		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			std::cerr << "Error listing port: " << tmp << std::endl
				<< "	Device full path too long." << std::endl;
		}
	}
	return (portMap);
}

std::string getSerialPort() {
	std::map<int, std::string> serialDevices(listSerialDevices());
	if (serialDevices.empty())
		return (std::string());
	std::cout << "Select serial port with ESP32 transmitter: ";
	int selection(0);
	std::cin >> selection;
	auto deviceIterator = serialDevices.find(selection);
	if (deviceIterator == serialDevices.end()) {
		std::cerr << "Error: Selected index " << selection << " does not exist." << std::endl;
		return (std::string());
	}
	std::cout << "Selection: "<< selection << " >> " << deviceIterator->second << std::endl;
	return deviceIterator->second;
}

void handle_error()
{
	char lastError[1024];
	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		lastError,
		1024,
		NULL
	);
	std::cerr << lastError << std::endl;
}

SerialHandler::SerialHandler(WsServer & ref, std::string const &port)
	: wsServer(ref),
	hSerial(),
	fifo(-1),
	fifoKey(-1),
	port(port)
{}

SerialHandler::~SerialHandler() {
	CloseHandle(hSerial);
}

bool SerialHandler::setup() {
	// Handle
	hSerial = CreateFile(port.c_str(),
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
	);
	if (hSerial==INVALID_HANDLE_VALUE) {
		if (GetLastError()==ERROR_FILE_NOT_FOUND) {
			std::cerr << "Failed to initialize serial connection." << std::endl;
			std::cerr << "\033[1;31mMake sure the ESP32 is connected\033[0m" << std::endl;
			return false;
		}
		std::cerr << "Error: some other error occurred." << std::endl;
		return false;
	}

	// Params
	DCB dcbSerialParams{};
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		std::cerr << "Error: error getting state." << std::endl;
		return false;
	}
	dcbSerialParams.BaudRate = BAUD_RATE;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams)) {
		std::cerr << "Error: error getting serial port state." << std::endl;
		return false;
	}

	// Timeouts
	COMMTIMEOUTS timeouts{};
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (!SetCommTimeouts(hSerial, &timeouts)) {
		std::cerr << "Error: error setting timeout." << std::endl;
		return false;
	}
	::signal(SIGINT, ddd::sighandler);
	return true;
}

void SerialHandler::parseTeleMsg(char* msg) {
	int const id = static_cast<int>(msg[0]);
	int const index = static_cast<int>(msg[1]);

	// Print id, index, and additional bytes
	// std::cout << "id = " << id
	//           << "  index = " << index
	//           << "  byte 3 = " << static_cast<int>(msg[2])
	//           << "  byte 4 = " << static_cast<int>(msg[3])
	//           << std::endl;

	if (id == 1) {
		t_tel_bat bat = *reinterpret_cast<t_tel_bat*>(&msg[0]);

		std::stringstream ss;

		ss << index << "{\"bat\":[" << bat.Bat_volt << "," 
					<< bat.Bat_cur << "]}";

		sendFront(ss.str().c_str());

		// std::cout << "id = " << id
		//       << "  index = " << index
		//       << "  byte 3 = " << static_cast<int>(msg[])
		//       << "  byte 4 = " << static_cast<int>(msg[3])
		//       << std::endl;

	}
	else if (id == 2) {


	}

	else if (id == 3) {
		t_tel_rc rc = *reinterpret_cast<t_tel_rc*>(&msg[0]);

		// std::cout << rc.ch_pitch << " " << rc.ch_roll << " " << rc.aux1 << std::endl; 

		std::stringstream ss;

		ss << index << "{\"rc\":[" << rc.ch_1 << "," << rc.ch_2 << "," 
					<< rc.ch_3 << "," << rc.ch_4 << "," << rc.ch_5 << "]}";

		sendFront(ss.str().c_str());
	}
	else 
				std::cout << msg << std::endl;
	{}

}

void SerialHandler::monitorIncoming() {
	char buf[BUFFER_SIZE] = {0};
	DWORD dwBytesRead = 0;
	std::string msg;

	while (!ddd::g_stop) {
		Sleep(500);
		if (!ReadFile(hSerial, buf, BUFFER_SIZE - 1, &dwBytesRead, NULL)) {
			handle_error();
		}
		buf[dwBytesRead] = 0;
		msg = buf;
		if (!msg.empty())
			parseTeleMsg(buf);
		std::memset(buf, 0, sizeof(buf));
	}
}

int SerialHandler::send(std::string const &msg) {
	DWORD dwBytesWritten = 0;
	{
		std::lock_guard<std::mutex> guard(serialMutex);
		if (!WriteFile(hSerial, msg.c_str(),  msg.length(), &dwBytesWritten, NULL)) {
			handle_error();
		}
	}
	this->sendFront(msg);
	return (dwBytesWritten);
}

int SerialHandler::sendFront(std::string const &msg) {

	// FOR NOW JUST ECHO ANY SERIAL SEND TO THE FRONT
	// std::lock_guard<std::mutex> guardWs(wsServer.wsMutex); // TODO look into the mutex.
	if (wsServer.wsConn) {
		wsServer.wsConn->send_text(msg.c_str());
	}
	return (1);
}
