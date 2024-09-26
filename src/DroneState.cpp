#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h> // write()

#include "DroneState.hpp"

namespace DroneController {
extern "C" void sig_handler(int sig) {
  (void)sig;
  g_stopme = 1;
}
} // namespace DroneController

std::ostream &operator<<(std::ostream &os, SetPoint &p) {
  os << "{'setpoint': [" << p.x << "," << p.y << "," << p.z << "]}"
     << std::endl;
  return os;
}

DroneState::DroneState() : index(0), armed(false), json() {
}

DroneState::DroneState(int idx) : index(idx), armed(false), json() {
  json["armed"] = "false";
  json["pos"] = "[0,0,0,0]";
  json["vel"] = "[0,0,0]";
  json["setpoint"] = "[0,0,0]";
  // json["pid"] = "";
  // json["trim"] = "";
}

DroneState::DroneState(DroneState const &cpy)
    : index(cpy.index), armed(cpy.armed), json(cpy.json) {}

DroneState &DroneState::operator=(DroneState const &rhs) {
  if (this == &rhs)
    return *this;
  this->index = rhs.index;
  this->armed = rhs.armed;
  this->json = rhs.json;
  return *this;
}

DroneState::~DroneState() {}

std::string DroneState::get_state() const {
  std::ostringstream ss;
  ss << "{";
  for (std::map<std::string, std::string>::const_iterator it =
           this->json.begin();
       it != this->json.end(); ++it) {
    ss << "'" << it->first << "'";
    ss << ":" << it->second << ",";
  }
  ss << "}";
  return (ss.str());
}

bool DroneState::send_state(int serial_port) {
  std::string state(std::to_string(this->index));
  state.append(this->get_state());
  return (write(serial_port, state.c_str(), state.length()) > 0);
}

ssize_t DroneState::send(int serial_port, std::string const &msg) {
  std::cout << "Port: " << serial_port << " msg: " << msg
            << " Len: " << msg.length() << std::endl;
  return write(serial_port, msg.c_str(), msg.length());
}

void DroneState::arm() {
  this->armed = true;
  this->json["armed"] = "true";
}

void DroneState::disarm() {
  this->armed = false;
  this->json["armed"] = "false";
}

void DroneState::setpoint(SetPoint const &point) {
  std::stringstream ss;
  ss << "[" << point.x << "," << point.y << "," << point.z << "]";
  this->json["setpoint"] = ss.str();
}

void DroneState::setpos(float x, float y, float z, float yaw) {
  std::stringstream ss;
  ss << "[" << x << "," << y << "," << z << "," << yaw << "]";
  this->json["pos"] = ss.str();
}

void DroneState::trim(float x, float y, float z, float yaw) {
  std::stringstream ss;
  ss << "[" << x << "," << y << "," << z << "," << yaw << "]";
  this->json["trim"] = ss.str();
}
