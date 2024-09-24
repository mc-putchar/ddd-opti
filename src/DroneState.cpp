#include "DroneState.hpp"
#include <unistd.h>  // write(), read(), close()

DroneState::DroneState()
{}

DroneState::DroneState(DroneState const &cpy)
{
  (void)cpy;
}

DroneState &DroneState::operator=(DroneState const &rhs)
{
  (void)rhs;
  return *this;
}

DroneState::~DroneState()
{}


std::string DroneState::arm()
{
  this->armed = true;
  return std::string("0{'armed': True}");
}

std::string DroneState::disarm()
{
  this->armed = true;
  return std::string("0{'armed': False}");
}


ssize_t DroneState::send(int serial_port, std::string const &msg)
{
  return write(serial_port, msg.c_str(), msg.length());
}
