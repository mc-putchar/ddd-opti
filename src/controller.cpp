// Linux headers
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
#include <chrono>

#include <iostream>
#include <string>
#include <cstring>

#include "DroneState.hpp"

#define VERSION 0.1
#define SERIAL_PORT "/dev/ttyUSB0"
#define PIPE_NAME "/tmp/ddd-data-interchange"
#define BUFFER_SIZE 512
#define MIN_INTER_SEND 1.5

static volatile std::sig_atomic_t g_stopped(0);

extern "C" int setup_serial() {
  int serial_port = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC);
  if (serial_port < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
    return (-1);
  }
  // Read in existing settings, and handle any error
  // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
  // must have been initialized with a call to tcgetattr() overwise behaviour
  // is undefined
  struct termios tty;
  if (tcgetattr(serial_port, &tty) != 0) {
    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    return (-1);
  }
  struct termios {
    tcflag_t c_iflag; /* input mode flags */
    tcflag_t c_oflag; /* output mode flags */
    tcflag_t c_cflag; /* control mode flags */
    tcflag_t c_lflag; /* local mode flags */
    cc_t c_line;      /* line discipline */
    cc_t c_cc[NCCS];  /* control characters */
  };
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
  // Our ESP32 transmitter is set to 1000000 baud rate
  cfsetispeed(&tty, 1000000);
  cfsetospeed(&tty, 1000000);
  // Save tty settings, also checking for error
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    return (-1);
  }
  return (serial_port);
}

void transmit(int serial, int pipe) {
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
    struct timeval timeout;
    timeout.tv_sec = 1;  // Wait for 1 second
    timeout.tv_usec = 0;

    int activity = select(pipe + 1, &read_fds, nullptr, nullptr, &timeout);
    if (activity < 0) {
      std::cerr << "Select error." << std::endl;
      continue;
      }

    if (FD_ISSET(pipe, &read_fds)) {
      rb = read(pipe, buf, BUFFER_SIZE - 1);
      if (rb > 0) {
        buf[rb] = 0;
        // TODO: update state
        msg = buf;
        std::cout << "Client sent: " << msg << std::endl;
        std::memset(buf, 0, sizeof(buf));
        lastMsg = msg; // Store the last message
        std::cout << "Msg update:  " << msg << std::endl;
        if (!msg.empty()) {
          // wb = write(serial, msg.c_str(), msg.length());
          (void)serial;
          wb = 5;
          if (wb < 0) {
            std::cerr << "Failed to send serial data to transmitter." << std::endl;
          }
          else {
            std::cout << "Sent " << wb << " bytes to transmitter.\n       Msg: " << msg << "\n" << std::endl;
            lastSendTime = std::chrono::steady_clock::now();
          }
        }
      }
      else if (rb < 0) {
        std::cerr << "Error reading from pipe." << std::endl;
      }
    }
    else {
      auto currentTime = std::chrono::steady_clock::now();
      if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastSendTime).count() >= MIN_INTER_SEND) {
          std::cout << "No new message. Sending last message:" << std::endl;
          // wb = write(serial, lastMsg.c_str(), lastMsg.length());
          (void)serial;
          wb = 5;
            if (wb < 0) {
              std::cerr << "Failed to send serial data to transmitter." << std::endl;
            } else {
              std::cout << "Sent " << wb << " bytes to transmitter.\n       Msg: " << lastMsg << "\n" << std::endl;
            }
            lastSendTime = currentTime; // Update the last send time
      }
    }
    usleep(500);
    wb = 0;
  }
}

int main(int ac, char **av) {
  // TODO: Implement flags
  (void)ac;
  (void)av;
  // setup serial port connection to transmitter
  // int const serial_port = setup_serial();
  int const serial_port = 5;
  if (serial_port < 0) {
    std::cerr << "Failed to setup serial connection on " << SERIAL_PORT
              << ". Ensure it is properly connected." << std::endl;
    return (1);
  }
  // setup communication with client
  errno = 0;
  if (mkfifo(PIPE_NAME, 0660)) {
    unlink(PIPE_NAME);
    if (mkfifo(PIPE_NAME, 0660)) {
      std::cerr << "Failed to create named pipe:" << strerror(errno)
                << std::endl;
      return (close(serial_port), 1);
    }
  }
  errno = 0;
  int const fifo = open(PIPE_NAME, O_RDONLY | O_NONBLOCK);
  if (fifo < 0) {
    std::cerr << "Failed to open named pipe: " << strerror(errno) << std::endl;
    return (close(serial_port), 1);
  }
  // TODO:
  // continuously send state to transmitter and
  // listen for state updates from client
  transmit(serial_port, fifo);

  // cleanup and exit
  close(serial_port);
  close(fifo);
  unlink(PIPE_NAME);
  return (0);
}
