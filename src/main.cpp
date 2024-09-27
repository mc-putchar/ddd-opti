#include <iostream>
#include <ostream>
#include <unistd.h>
#include <vector>
// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <errno.h>   // Error integer and strerror() function
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions

#include "DroneState.hpp"

#define X_LIMIT 4
#define Y_LIMIT 2
#define Z_LIMIT 4
#define SAFETY_MARGIN 0.2
#define SERIAL_PORT "/dev/ttyUSB0"

extern "C" int setup_serial() {
  int serial_port = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_SYNC);

  // Check for errors
  if (serial_port < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
    return (-1);
  }
  // Create new termios struct, we call it 'tty' for convention
  // No need for "= {0}" at the end as we'll immediately write the existing
  // config to this struct
  struct termios tty;

  // Read in existing settings, and handle any error
  // NOTE: This is important! POSIX states that the struct passed to tcsetattr()
  // must have been initialized with a call to tcgetattr() overwise behaviour
  // is undefined
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
  // B0,  B50,  B75,  B110,  B134,  B150,  B200, B300, B600, B1200, B1800,
  // B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800
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

void TT_RigidBodyLocation(int rbIndex,                  //== RigidBody Index
                          float *x, float *y, float *z, //== Position
                          float *qx, float *qy, float *qz,
                          float *qw,                             //== Quaternion
                          float *yaw, float *pitch, float *roll) //== Euler
{
  (void)rbIndex;
  *x = 0;
  *y = 0;
  *z = 0;
  *qx = *qy = *qz = *qw = 0;
  *yaw = *pitch = *roll = 0;
}

#define BUFSIZE 4096
#define PROMPT "Input drone command: "
int loop(int port) {
  int run(1);
  char buff[BUFSIZE];
  ssize_t r;
  std::string cmd;
  DroneState drone = DroneState();
  while (run) {
    std::cout << PROMPT;
    std::getline(std::cin, cmd);
    if (cmd == "exit" || cmd == "quit" || cmd == "q")
      break;
    if (cmd == "arm" || cmd == "a") {
      if (drone.send(port, "0" + drone.arm()) < 0) {
        std::cerr << "Failed to send" << std::endl;
        continue;
      }
      std::cout << "Drone armed" << std::endl;
    } else if (cmd == "disarm" || cmd == "d") {
      if (drone.send(port, "0" + drone.disarm()) < 0) {
        std::cerr << "Failed to send" << std::endl;
        continue;
      }
      std::cout << "Drone disarmed" << std::endl;
    } else if (cmd == "set" || cmd == "s") {
      std::cout << "Setpoint(x y z): ";
      std::string tmp;
      std::getline(std::cin, tmp);
      SetPoint point(tmp);
      if (drone.send(port, "0" + drone.setpoint(point)) < 0) {
        std::cerr << "Failed to send" << std::endl;
        continue;
      }
      std::cout << "New target: " << point.x << ", " << point.y << ", "
                << point.z << std::endl;
    } else if (cmd == "r") {
      if ((r = read(port, buff, BUFSIZE - 1)) > 0) {
        buff[r] = 0;
        std::cout << "serial: " << buff << std::endl;
      }
    } else if (cmd == "c" || cmd == "check") {
      std::cout << "Drone state: " << drone.get_state() << std::endl;
    } else if (cmd == "t" || cmd == "trim") {
      std::cout << "Trim(x y z yaw): ";
      std::string tmp;
      std::getline(std::cin, tmp);
      float x, y, z, yaw;

      std::istringstream iss(tmp);
      iss >> x;
      iss >> y;
      iss >> z;
      iss >> yaw;
      if (drone.send(port, "0" + drone.trim(x, y, z, yaw)) < 0) {
        std::cerr << "Failed to send" << std::endl;
        continue;
      }
      std::cout << "Set trim to: " << x << ", " << y << ", " << z << ", " << yaw
                << std::endl;
    } else {
      std::cout << "Command not recognized" << std::endl;
    }
    if ((r = read(port, buff, BUFSIZE - 1)) > 0) {
      buff[r] = 0;
      std::cout << "serial: " << buff << std::endl;
    }
  }
  std::cout << "Exiting program" << std::endl;
  return 0;
}

int main() {
  int serial_port;
  float x, y, z;
  float qx, qy, qz, qw;
  float yaw, pitch, roll;

  TT_RigidBodyLocation(0, &x, &y, &z, &qx, &qy, &qz, &qw, &yaw, &pitch, &roll);
  std::cout << "x: " << x << " y: " << y << " z: " << z << std::endl
            << "qx: " << qx << " qy: " << qy << " qz: " << qz << " qw: " << qw
            << std::endl
            << "yaw: " << yaw << " pitch: " << pitch << " roll: " << roll
            << std::endl;
  if ((serial_port = setup_serial()) < 0)
    return (1);
  // SetPoint point = SetPoint(x, y, z, yaw, pitch, roll);
  // DroneState test_drone = DroneState();
  // std::cout << test_drone.setpoint(point) << std::endl;
  // std::cout << test_drone.setpos(0, 0, 0, 0) << std::endl;
  loop(serial_port);
  return (0);
}
