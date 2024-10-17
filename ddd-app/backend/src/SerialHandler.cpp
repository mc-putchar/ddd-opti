#include "SerialHandler.hpp"

SerialHandler::SerialHandler(std::mutex & wsmutex, crow::websocket::connection*& wsconn)
	: g_stopped(false),
	serial_port(-1),
	fifo(-1),
	fifoKey(-1),
	wsMutex(wsmutex),
	wsConn(wsconn)
	{}


SerialHandler::~SerialHandler() {
	cleanup();
}


bool SerialHandler::setup() {
	serial_port = setup_serial();
	if (serial_port < 0) return false;
	return true;
	fifo = createNamedPipe(PIPE_NAME_CMD_LINE);
	fifoKey = createNamedPipe(PIPE_NAME_KEY_HOOK);
	return fifo >= 0 && fifoKey >= 0;
}

void SerialHandler::cleanup() {
	if (serial_port >= 0) close(serial_port);
	if (fifo >= 0) close(fifo);
	if (fifoKey >= 0) close(fifoKey);
	unlink(PIPE_NAME_CMD_LINE);
	unlink(PIPE_NAME_KEY_HOOK);
}


int SerialHandler::setup_serial() {
	std::cout << "Attempting to open serial port: " << SERIAL_PORT << std::endl;
	int serial_port = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC);
	if (serial_port < 0) {
		printf("Error %i from open: %s\n", errno, strerror(errno));
		return (-1);
	}
	std::cout << "Serial port opened successfully." << std::endl;

	struct termios tty;
	if (tcgetattr(serial_port, &tty) != 0) {
		printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
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
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
		return (-1);
	}
	return (serial_port);
}

void SerialHandler::send_to_ws(std::string msg) {
	std::lock_guard<std::mutex> guard(wsMutex);
	if (wsConn) {
		wsConn->send_text(msg.c_str()); // Send message through WebSocket connection
	} else {
		std::cout << "WebSocket connection is not available." << std::endl;
	}
}

void SerialHandler::transmit(int pipe, int pipeKey) {
	ssize_t wb(0);
	ssize_t rb(0);
	char buf[BUFFER_SIZE];
	std::string msg;
	std::string lastMsg;
	DroneState drone = DroneState(0);

	auto lastSendTime = std::chrono::steady_clock::now(); // Track when the last message was sent

	while (!g_stopped) {
		fd_set read_fds;
		FD_ZERO(&read_fds);
		FD_SET(pipe, &read_fds);
		FD_SET(pipeKey, &read_fds);
		if (serial_port >= 0) // in case there is no ESP32 connected
			FD_SET(serial_port, &read_fds);
		struct timeval timeout;
		timeout.tv_sec = 1;  // Wait for 1 second
		timeout.tv_usec = 0;

		int max_fd = std::max(pipeKey, serial_port); // Get the highest file descriptor value
		int activity = select(max_fd + 1, &read_fds, nullptr, nullptr, &timeout);
		if (activity < 0) {
			std::cerr << "Select error." << std::endl;
		continue;
		}

		if (FD_ISSET(pipe, &read_fds)) {
			rb = read(pipe, buf, BUFFER_SIZE - 1);
			if (rb > 0) {
				buf[rb] = 0;
				msg = buf;
				std::memset(buf, 0, sizeof(buf));
				if (!msg.empty()) {
					wb = write(serial_port, msg.c_str(), msg.length());
					if (wb < 0) {
						std::cerr << "Failed to send serial data to transmitter." << std::endl;
					}
					else {
						std::cout << "Sent " << wb << " bytes to transmitter.\nMsg:  " << msg << std::endl;
						lastSendTime = std::chrono::steady_clock::now();
					}
					send_to_ws(msg);
				}
			}
			else if (rb < 0) {
				std::cerr << "Error reading from pipe." << std::endl;
			}
		}
		if (FD_ISSET(pipeKey, &read_fds)) {
			rb = read(pipeKey, buf, BUFFER_SIZE - 1);
			if (rb > 0) {
				buf[rb] = 0;
				msg = buf;
				std::memset(buf, 0, sizeof(buf));
				if (!msg.empty()) {
				wb = write(serial_port, msg.c_str(), msg.length());
				if (wb < 0) {
					std::cerr << "Failed to send serial data to transmitter." << std::endl;
				}
				else {
					std::cout << "Sent " << wb << " bytes to transmitter.\nMsg:  " << msg << std::endl;
					lastSendTime = std::chrono::steady_clock::now();
				}
				send_to_ws(msg);
				}
			}
			else if (rb < 0) {
				std::cerr << "Error reading from pipe." << std::endl;
			}
		}

		if (FD_ISSET(serial_port, &read_fds)) {
			rb = read(serial_port, buf, BUFFER_SIZE - 1);
			if (rb > 0) {
				buf[rb] = 0;
				msg = buf;
				std::memset(buf, 0, sizeof(buf));
				if (!msg.empty())
					std::cout << msg << std::endl;
				}
			else if (rb < 0) {
				std::cerr << "Error reading from serial." << std::endl;
			}
		}

		auto currentTime = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastSendTime).count() >= MIN_INTER_SEND) {
			const char* ping = "0{'ping'}"; // HOW WILL WE HANDLE MULTIPLE PING DRONES MONITORING?
			wb = write(serial_port, ping, strlen(ping));
			if (wb < 0) {
				std::cerr << "Failed to send serial data to transmitter." << std::endl;
			} else {
				std::cout << "Pinged the receiver" << std::endl;
				send_to_ws(ping);
			}
			lastSendTime = currentTime; // Update the last send time
		}
		
		usleep(500);
		wb = 0;
	}
}

int SerialHandler::createNamedPipe(std::string namePipe) {
	if (mkfifo(namePipe.c_str(), 0660) != 0 && errno != EEXIST) return -1;
	return open(namePipe.c_str(), O_RDONLY | O_NONBLOCK);
}

