#include <iostream>
#include <ostream>
#include <vector>
// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <errno.h>   // Error integer and strerror() function
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#define X_LIMIT 4
#define Y_LIMIT 2
#define Z_LIMIT 4
#define SAFETY_MARGIN 0.2

extern "C" void serial_write() {
  int serial_port = open("/dev/ttyUSB0", O_RDWR);

  // Check for errors
  if (serial_port < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
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
  }
  struct termios {
    tcflag_t c_iflag; /* input mode flags */
    tcflag_t c_oflag; /* output mode flags */
    tcflag_t c_cflag; /* control mode flags */
    tcflag_t c_lflag; /* local mode flags */
    cc_t c_line;      /* line discipline */
    cc_t c_cc[NCCS];  /* control characters */
  };
  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag |= PARENB;  // Set parity bit, enabling parity
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in
                          // communication (most common)
  tty.c_cflag |= CSTOPB;  // Set stop field, two stop bits used in communication
  // tty.c_cflag &= ~CSIZE; // Clear all the size bits, then use one of the
  // statements below
  tty.c_cflag |= CS5; // 5 bits per byte
  tty.c_cflag |= CS6; // 6 bits per byte
  tty.c_cflag |= CS7; // 7 bits per byte
  tty.c_cflag |= CS8; // 8 bits per byte (most common)
  tty.c_cflag &=
      ~CRTSCTS;           // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CRTSCTS; // Enable RTS/CTS hardware flow control
  tty.c_cflag |=
      CREAD | CLOCAL;     // Turn on READ & ignore ctrl lines (CLOCAL = 1)
  tty.c_lflag &= ~ECHO;   // Disable echo
  tty.c_lflag &= ~ECHOE;  // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                   ICRNL); // Disable any special handling of received bytes
  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g.
                         // newline chars)
  tty.c_oflag &=
      ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT
  // PRESENT IN LINUX) tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars
  // (0x004) in output (NOT PRESENT IN LINUX)
  tty.c_cc[VTIME] = 10; // Wait for up to 1s (10 deciseconds), returning as soon
                        // as any data is received.
  tty.c_cc[VMIN] = 0;
  // Set in/out baud rate to be 9600
  // B0,  B50,  B75,  B110,  B134,  B150,  B200, B300, B600, B1200, B1800,
  // B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800
  cfsetispeed(&tty, B115200);
  cfsetospeed(&tty, B115200);
  // Save tty settings, also checking for error
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
  }
  // unsigned char msg[] = {'0', '{', 'l', 'l', 'o', '\r'};
  static const char msg[] = "0{'setpoint': [0,0,0]}";
  if (write(serial_port, msg, sizeof(msg)))
    printf("%s\n", msg);
}
struct SetPoint {
  float x, y, z;
  float yaw, pitch, roll;
  int stoptime;
  SetPoint(float x, float y, float z, float yaw, float pitch, float roll)
      : x(x), y(y), z(z), yaw(yaw), pitch(pitch), roll(roll) {}
};

class Trajectory {
public:
  std::vector<SetPoint> points;
};

std::ostream &operator<<(std::ostream &os, SetPoint &p) {
  os << "0{'setpoint': [" << p.x << "," << p.y  << "," << p.z << "]}" << std::endl;
  return os;
}
void TT_RigidBodyLocation(int rbIndex,                  //== RigidBody Index
                          float *x, float *y, float *z, //== Position
                          float *qx, float *qy, float *qz,
                          float *qw,                             //== Quaternion
                          float *yaw, float *pitch, float *roll) //== Euler
{
  *x = 0;
  *y = 0;
  *z = 0;
  *qx = *qy = *qz = *qw = 0;
  *yaw = *pitch = *roll = 0;
}

int main() {
  float x, y, z;
  float qx, qy, qz, qw;
  float yaw, pitch, roll;

  TT_RigidBodyLocation(0, &x, &y, &z, &qx, &qy, &qz, &qw, &yaw, &pitch, &roll);
  std::cout << "x: " << x << " y: " << y << " z: " << z << std::endl
            << "qx: " << qx << " qy: " << qy << " qz: " << qz << " qw: " << qw
            << std::endl
            << "yaw: " << yaw << " pitch: " << pitch << " roll: " << roll
            << std::endl;
  serial_write();
  SetPoint point = SetPoint(x, y, z, yaw, pitch, roll);
  std::cout << "SetPoint: " << point << std::endl;
  return (0);
}
