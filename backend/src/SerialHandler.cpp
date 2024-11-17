#include <string>

#include "Log.hpp"
#include "SerialHandler.hpp"

#define TAG	"Serial"

std::string getSerialPort()
{
	return SERIAL_PORT;
}

SerialHandler::SerialHandler(std::string const &port)
	: port(port), port_fd(-1)
{}


SerialHandler::~SerialHandler() {
	(void)close(port_fd);
}


bool SerialHandler::setup() {
	port_fd = setup_serial();
	if (port_fd < 0) {
		ERROR(TAG, "Failed to initialize serial connection. Make sure the ESP32 is connected");
		// std::cerr << "Failed to initialize serial connection." << std::endl;
		// std::cerr << "\033[1;31mMake sure the ESP32 is connected\033[0m" << std::endl;
		return false;
	}
	// std::cerr << "Serial port connected." << std::endl;
	INFO(TAG, "Serial port connected.");
	return true;
}


int SerialHandler::setup_serial() {
	// std::cout << "Connecting to serial port: " << SERIAL_PORT << std::endl;
	std::string tmp("Connecting to serial port: ");
	tmp.append(SERIAL_PORT);
	INFO(TAG, tmp.c_str());
	int serial_port = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC);
	if (serial_port < 0) {
		// std::cerr << "Error " << errno << " from open: " 
		// 	<< strerror(errno) << std::endl;
		ERROR(TAG, "Failed to open requested port.");
		return (-1);
	}
	struct termios tty;
	if (tcgetattr(serial_port, &tty) != 0) {
		// std::cerr << "Error " << errno << " from tcgetattr: " 
		// 	<< strerror(errno) << std::endl;
		ERROR(TAG, "Failed to get term attributes.");
		return (-1);
	}

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit characters
	tty.c_iflag &= ~IGNBRK;                     // disable break processing
	tty.c_lflag = 0;     // no signaling chars, no echo, no canonical processing
	tty.c_oflag = 0;     // no remapping, no delays
	tty.c_cc[VMIN] = 0;  // read doesn't block
	tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout
	tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
	tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls, enable reading
	tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
	tty.c_cflag &= ~CSTOPB;
	tty.c_cflag &= ~CRTSCTS;

	cfsetispeed(&tty, BAUD_RATE);
	cfsetospeed(&tty, BAUD_RATE);

	if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
		// std::cerr << "Error " << errno << " from tcsetattr: " 
		// 	<< strerror(errno) << std::endl;
		ERROR(TAG, "Failed to set term attributes.");
		return (-1);
	}
	return (serial_port);
}

void SerialHandler::parseTeleMsg(char* msg) {
	int const id = static_cast<int>(msg[0]);
	int const index = static_cast<int>(msg[1]);

	if (id == S_TEL_BAT) {
		t_tel_bat bat = *reinterpret_cast<t_tel_bat*>(&msg[0]);

		std::stringstream ss;

		ss << index << "{\"bat\":[" << bat.volt << "," 
					<< bat.cur << "]}";

		sendFront(ss.str());
	}
	else if (id == S_TEL_ATITU) {
		t_tel_atitu atitu = *reinterpret_cast<t_tel_atitu*>(&msg[0]);
		std::stringstream ss;
		ss << index << "{\"ati\":[" << atitu.pitch << "," 
					<< atitu.roll << "," 
					<< atitu.yaw << "]}";

		sendFront(ss.str());
	}

	else if (id == S_TEL_RC) {
		t_tel_rc rc = *reinterpret_cast<t_tel_rc*>(&msg[0]);

		std::stringstream ss;

		ss << index << "{\"rc\":[" << rc.ch_1 << "," << rc.ch_2 << "," 
					<< rc.ch_3 << "," << rc.ch_4 << "," << rc.ch_5 << "]}";

		sendFront(ss.str());
	}
	else
		std::cout << msg << std::endl;
}

void SerialHandler::monitorIncoming() {
	ssize_t rb(0);
	char buf[BUFFER_SIZE];
	std::string msg;

	while (true) {
		usleep(500);
		if (port_fd < 0) continue;
		std::memset(buf, 0, sizeof(buf));
		rb = read(port_fd, buf, BUFFER_SIZE - 1);
		if (rb < 0) {
			std::cerr << "Error reading from serial." << std::endl;
			ERROR(TAG, "Error reading from serial.");
			continue;
		}
		buf[rb] = 0;
		msg = buf;
		if (!msg.empty())
			parseTeleMsg(buf);
	}
}

int SerialHandler::send(std::string const &msg) {

	int serial_rt;
	{
		std::lock_guard<std::mutex> guard(serialMutex);
		serial_rt = write(port_fd, msg.c_str(), msg.length());
	}
	this->sendFront(msg);
	return (serial_rt);
}

int SerialHandler::sendFront(std::string const &msg) {

	// FOR NOW JUST ECHO ANY SERIAL SEND TO THE FRONT
	// std::lock_guard<std::mutex> guardWs(wsServer.wsMutex); // TODO look into the mutex.
	// if (wsServer.wsConn) {
	// 	wsServer.wsConn->send_text(msg.c_str());
	// }
	WsServer::getInstance().send(msg);
	return (1);
}
