#include <iostream>
#include <sstream>
#include <unistd.h> // write()

#include "DroneState.hpp"

std::ostream &operator<<(std::ostream &os, SetPoint &p) {
  os << "{'setpoint': [" << p.x << "," << p.y  << "," << p.z << "]}" << std::endl;
  return os;
}

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

ssize_t DroneState::send(int serial_port, std::string const &msg) {
  std::cout << "Port: " << serial_port << " msg: " << msg << " Len: " << msg.length() << std::endl;
  return write(serial_port, msg.c_str(), msg.length());
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

std::string DroneState::setpoint(SetPoint const &point)
{
  std::stringstream ss;
  ss << "{'setpoint':[" << point.x << "," << point.y << "," << point.z << "]}";
  return ss.str();
}

std::string DroneState::setpos(float x, float y, float z, float yaw)
{
  std::stringstream ss;
  ss << "{'pos':[" << x << "," << y << "," << z << "," << yaw << "]}";
  return ss.str();
}

std::string DroneState::trim(float x, float y, float z, float yaw)
{
  std::stringstream ss;
  ss << "{'trim':[" << x << "," << y << "," << z << "," << yaw << "]}";
  return ss.str();
}

