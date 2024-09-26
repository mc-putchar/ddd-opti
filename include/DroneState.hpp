#ifndef DRONESTATE_HPP
#define DRONESTATE_HPP

#include <csignal>
#include <map>
#include <sstream>
#include <string>

namespace DroneController {
volatile static sig_atomic_t g_stopme(0);
extern "C" void sig_handler(int sig);
}

struct SetPoint {
  float x, y, z;
  float yaw, pitch, roll;
  SetPoint(float x, float y, float z, float yaw, float pitch, float roll)
      : x(x), y(y), z(z), yaw(yaw), pitch(pitch), roll(roll) {}
  SetPoint(std::string setpoint) {
    std::istringstream iss(setpoint);
    iss >> this->x;
    iss >> this->y;
    iss >> this->z;
  }
};

// NOTE: possible keys:
// armed = bool
// pos = array[4]
// vel = array[3]
// setpoint = array[3]
// pid = array[17]
// trim = array[4]
class DroneState {
public:
  DroneState();
  DroneState(int idx);
  DroneState(DroneState const &cpy);
  DroneState &operator=(DroneState const &rhs);
  ~DroneState();

  std::string get_state() const;
  bool send_state(int serial_port);
  ssize_t send(int serial_port, std::string const &msg);
  void arm();
  void disarm();
  void setpoint(SetPoint const &point);
  void setpos(float x, float y, float z, float yaw);
  void trim(float x, float y, float z, float yaw);

private:
  int index;
  bool armed;
  std::map<std::string, std::string> json;
};

#endif // DRONESTATE_HPP
