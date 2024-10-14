#include <iostream>
#include <sstream>
#include <string>

#include <unistd.h>

#include "DroneState.hpp"
#define INCREMENTATION 0.3
#define TRIM_INCREMENTATION 25

DroneState::DroneState() : index(0), armed(false), json(), _x(0), _y(0), _z(0), _yaw(0), _Tx(0), _Ty(0), _Tz(0), _Tyaw(0) {}

DroneState::DroneState(int idx) : index(idx), armed(false), json(), _x(0), _y(0), _z(0), _yaw(0) {}

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
  // NOTE: hardcoded roll trim value for current test drone
  str.append(this->trim(0, 64, -800, 0));
  if (this->send(serial_port, str) < 0) {
    std::cerr << "Failed to send throttle down signal" << std::endl;
    return false;
  }
  this->json["trim"] = "[0,64,-800,0]";
  str = std::to_string(this->index);
  str.append(this->arm());
  sleep(1);
  if (this->send(serial_port, str) < 0) {
    std::cerr << "Failed to send arming signal" << std::endl;
    return false;
  }
  sleep(1);
  str = std::to_string(this->index);
str.append(this->trim(0, 64, 0, 0));
  if (this->send(serial_port, str) < 0) {
    std::cerr << "Failed to reset throttle" << std::endl;
    return false;
  }
  this->json["trim"] = "[0,64,0,0]";
  _Ty = 64;

//   sleep(1);
//   if (this->send(serial_port, "0{'setpoint':[0,0,0.5]") < 0)
//     std::cerr << "Failed to send" << std::endl;
//   usleep(500000);
//   if (this->send(serial_port, "0{'pos':[0,0,0.5,0]") < 0)
//     std::cerr << "Failed to send" << std::endl;
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
  _x = x;
  _y = y;
  _z = z;
  _yaw = yaw;
  this->json["pos"] = ss.str();
  return std::string("{'pos':" + ss.str() + "}");
}

std::string DroneState::adjustpos(std::string var, std::string change) {

  if (var == "x") {
	if (change == "+") 
	  _x += INCREMENTATION;
	if (change == "-") 
	  _x -= INCREMENTATION;
  }
  if (var == "y") {
	if (change == "+") 
	  _y += INCREMENTATION;
	if (change == "-") 
	  _y -= INCREMENTATION;
  }
  if (var == "z") {
    if (change == "+") 
	  _z += INCREMENTATION;
	if (change == "-") 
	  _z -= INCREMENTATION;
  }
  if (var == "yaw") {
     if (change == "+") 
	  _yaw += INCREMENTATION;
	if (change == "-") 
	  _yaw -= INCREMENTATION;
  }
  std::stringstream ss;
  ss << "[" << _x << "," << _y << "," << _z << "," << _yaw << "]";
  this->json["pos"] = ss.str();
  return std::string("{'pos':" + ss.str() + "}");
}

std::string DroneState::adjusttrim(std::string var, std::string change) {

  if (var == "x") {
	if (change == "+") 
	  _Tx -= TRIM_INCREMENTATION;
	if (change == "-") 
	  _Tx += TRIM_INCREMENTATION;
  }
  if (var == "y") {
	if (change == "+") 
	  _Ty -= TRIM_INCREMENTATION;
	if (change == "-") 
	  _Ty += TRIM_INCREMENTATION;
  }
  if (var == "z") {
    if (change == "+") 
	  _Tz -= TRIM_INCREMENTATION;
	if (change == "-") 
	  _Tz += TRIM_INCREMENTATION;
  }
  if (var == "yaw") {
     if (change == "+") 
	  _Tyaw -= TRIM_INCREMENTATION;
	if (change == "-") 
	  _Tyaw += TRIM_INCREMENTATION;
  }
  std::stringstream ss;
  ss << "[" << _Tx << "," << _Ty << "," << _Tz << "," << _Tyaw << "]";
  this->json["trim"] = ss.str();
  return std::string("{'trim':" + ss.str() + "}");
}

std::string DroneState::adjustlight(std::string var, std::string change) {

  if (var == "servo") {
	if (change == "+") 
	  _servoAngle -= 5;
	if (change == "-") 
	  _servoAngle += 5;
  }

  std::stringstream ss;
  ss << "[" << _servoAngle << "]";
  return std::string("{'servo':" + ss.str() + "}");
}

std::string DroneState::trim(float x, float y, float z, float yaw) {
  std::stringstream ss;
  ss << "[" << x << "," << y << "," << z << "," << yaw << "]";
  this->json["trim"] = ss.str();
  return std::string("{'trim':" + ss.str() + "}");
}
