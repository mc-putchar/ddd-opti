#ifndef SERIAL_HPP
#define SERIAL_HPP

#include "DroneState.hpp"
#include <iostream>
#include <string>
#include <vector>

#include <errno.h> // Error integer and strerror() function
#include <fcntl.h> // Contains file controls like O_RDWR
#include <stdio.h>
#include <string.h>
#include <termios.h> // Contains POSIX terminal control definitions

#define SERIAL_PORT "/dev/ttyUSB0"

class Serial {
public:
  int get_fd() const;
  bool setup();
  void handle_serial(std::vector<DroneState> &drones);

private:
  int fd;
};

#endif // SERIAL_HPP
