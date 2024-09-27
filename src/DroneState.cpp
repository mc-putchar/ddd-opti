#include <iostream>
#include <sstream>
#include <string>

#include <unistd.h>

#include "DroneState.hpp"

DroneState::DroneState() : index(0), armed(false), json() {}

DroneState::DroneState(int idx) : index(idx), armed(false), json() {}

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

bool DroneState::is_armed() const { return this->armed; }

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

bool DroneState::send_state(int fd) {
  std::string state(std::to_string(this->index));
  state.append(this->get_state());
  ssize_t wb(0);
  ssize_t rem(state.length());
  while (rem) {
    wb = write(fd, state.c_str(), state.length());
    if (wb < 0)
      return false;
    rem -= wb;
  }
  return true;
}

ssize_t DroneState::send(int fd, std::string const &msg) {
  return write(fd, msg.c_str(), msg.length());
}

// Startup sequence requires throttling down to safe margin
// to allow arming the drone
bool DroneState::startup(int serial_port) {
  std::string str(std::to_string(this->index));
  str.append(this->trim(0, 0, -800, 0));
  if (this->send(serial_port, str) < 0) {
    std::cerr << "Failed to send throttle down signal" << std::endl;
    return false;
  }
  this->json["trim"] = "[0,0,-800,0]";
  str = std::to_string(this->index);
  str.append(this->arm());
  sleep(1);
  if (this->send(serial_port, str) < 0) {
    std::cerr << "Failed to send arming signal" << std::endl;
    return false;
  }
  return true;
}

std::string DroneState::arm() {
  this->armed = true;
  this->json["armed"] = "true";
  return std::string("{'armed':true}");
}

std::string DroneState::disarm() {
  this->armed = false;
  this->json["armed"] = "false";
  return std::string("{'armed':false}");
}

std::string DroneState::setpoint(SetPoint const &point) {
  std::stringstream ss;
  ss << "[" << point.x << "," << point.y << "," << point.z << "]";
  this->json["setpoint"] = ss.str();
  return std::string("{'setpoint':" + ss.str() + "}");
}

std::string DroneState::setpos(float x, float y, float z, float yaw) {
  std::stringstream ss;
  ss << "[" << x << "," << y << "," << z << "," << yaw << "]";
  this->json["pos"] = ss.str();
  return std::string("{'pos':" + ss.str() + "}");
}

std::string DroneState::trim(float x, float y, float z, float yaw) {
  std::stringstream ss;
  ss << "[" << x << "," << y << "," << z << "," << yaw << "]";
  this->json["trim"] = ss.str();
  return std::string("{'trim':" + ss.str() + "}");
}
