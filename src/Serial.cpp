#include "Serial.hpp"
#include "DroneState.hpp"
#include <unistd.h>
#include <vector>

extern "C" int setup_serial() {
  int serial_port = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC);

  // Check for errors
  if (serial_port < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
    return (-1);
  }

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
  tty.c_lflag = 0;                            // no signaling chars, no echo, no
                                              // canonical processing
  tty.c_oflag = 0;                            // no remapping, no delays
  tty.c_cc[VMIN] = 0;                         // read doesn't block
  tty.c_cc[VTIME] = 5;                        // 0.5 seconds read timeout

  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

  tty.c_cflag |= (CLOCAL | CREAD);   // ignore modem controls,
                                     // enable reading
  tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
  tty.c_cflag &= ~CSTOPB;
  tty.c_cflag &= ~CRTSCTS;
  cfsetispeed(&tty, 1000000);
  cfsetospeed(&tty, 1000000);
  // Save tty settings, also checking for error
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    return (-1);
  }
  return (serial_port);
}

int Serial::get_fd() const { return this->fd; }

bool Serial::setup() {
  this->fd = setup_serial();
  return (this->fd >= 0);
}

// TODO: Fix handling exit signal correctly
using DroneController::g_stopme;
void Serial::handle_serial(std::vector<DroneState> &drones) {
  while (!g_stopme) {
    for (std::vector<DroneState>::iterator it = drones.begin();
         it != drones.end(); ++it) {
      if (!it->send_state(this->fd))
        std::cerr << "Failed to send state to transmitter" << std::endl;
      sleep(1);
    }
  }
}
