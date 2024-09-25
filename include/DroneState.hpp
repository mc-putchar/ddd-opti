#ifndef DRONESTATE_HPP
#define DRONESTATE_HPP

#include <map>
#include <sstream>
#include <string>

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

  ssize_t send(int serial_port, std::string const &msg);
  std::string arm();
  std::string disarm();
  std::string setpoint(SetPoint const &point);
  std::string setpos(float x, float y, float z, float yaw);
  std::string trim(float x, float y, float z, float yaw);

private:
  int index;
  bool armed;
  std::map<std::string, std::string> json;
};

#endif // DRONESTATE_HPP
