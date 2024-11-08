#include <winsock2.h>
#include <Windows.h>
#include <WinBase.h>

#include <csignal>
#include <iostream>
#include <map>
#include <string>

#include "WinSerialHandler.hpp"

namespace ddd {
	volatile std::sig_atomic_t	g_stop(0);
	void	sighandler(int sig) {
		(void)sig;
		g_stop = 1;
	}
}

std::string getSerialPort()
{
	char lpTargetPath[5000]; // buffer to store the path of the COM PORTS
	std::map<int, int> portMap;
	int idx(0);
	std::string str = "COM";
	std::cout << "Connected serial ports:" << std::endl;
	for (int i = 0; i < 255; i++) {
		std::string tmp = "COM" + std::to_string(i);
		DWORD res = QueryDosDevice(tmp.c_str(), lpTargetPath, 5000);
		if (res != 0) {
			portMap[++idx] = i;
			std::cout << idx << ": " << str + std::to_string(i) << ": " << lpTargetPath << std::endl;
		}
		if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			std::cerr << "Error listing ports" << std::endl;
			return "";
		}
	}
	int selection(-1);
	std::cout << "Select serial port with ESP32 transmitter: ";
	std::cin >> selection;
	auto port = portMap.find(selection);
	if (port == portMap.end()) {
		std::cerr << "Error: Selected index " << selection << " does not exist." << std::endl;
		return "";
	}
	str.append(std::to_string(port->second));
	std::cout << "Selection: "<< selection << " >> " << str << std::endl;
	return str;
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
		//some other error occurred. Inform user.
		return false;
	}

	DCB dcbSerialParams{};
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		//error getting state
		return false;
	}
	dcbSerialParams.BaudRate = BAUD_RATE;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams)) {
		//error setting serial port state
		return false;
	}

	COMMTIMEOUTS timeouts{};
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (!SetCommTimeouts(hSerial, &timeouts)) {
		//error occureed. Inform user
		return false;
	}
	::signal(SIGINT, ddd::sighandler);
	return true;
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
		std::memset(buf, 0, sizeof(buf));
		if (!msg.empty())
			std::cout << msg << std::endl;
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
	// FOR NOW JUST ECHO ANY SERIAL SEND TO THE FRONT
	// std::lock_guard<std::mutex> guardWs(wsServer.wsMutex); // TODO look into the mutex.
	if (wsServer.wsConn) {
		wsServer.wsConn->send_text(msg.c_str());
	}
	return (dwBytesWritten);
}
