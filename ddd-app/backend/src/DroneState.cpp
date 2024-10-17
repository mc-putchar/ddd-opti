#include <iostream>
#include <sstream>
#include <string>

#include <unistd.h>

#include "DroneState.hpp"
#define INCREMENTATION 0.3
#define TRIM_INCREMENTATION 25

DroneState::DroneState() 
	: path(nullptr),
		index(0),
		armed(false),
		position{0, 0, 0, 0},
		trim{0, 0, 0, 0},
		light{0, 0}
		{}


DroneState::DroneState(int idx)
	: path(nullptr),
		index(idx),
		armed(false),
		position{0, 0, 0, 0},
		trim{0, 0, 0, 0},
		light{0, 0}
		{}


DroneState::DroneState(DroneState const &cpy)
	: index(cpy.index),
		armed(cpy.armed),
		position{cpy.position.x, cpy.position.y, cpy.position.z, cpy.position.yaw},
		trim{cpy.trim.x, cpy.trim.y, cpy.trim.z, cpy.trim.yaw},
		light{cpy.light.power, cpy.light.angle} {}


DroneState &DroneState::operator=(DroneState const &rhs) {
	if (this == &rhs)
		return *this;
	this->index = rhs.index;
	this->armed = rhs.armed;
	this->position = rhs.position;
	this->trim = rhs.trim;
	this->light = rhs.light;
	return *this;
}


DroneState::~DroneState() {}

void DroneState::setPath(std::unique_ptr<Path> p) {
	path = std::move(p);
}


bool DroneState::is_armed() const { return this->armed; }


ssize_t DroneState::send(int fd, std::string const &msg) {
	std::ostringstream oss;
	oss << this->index << msg;  // Append index and message in a single step.
	std::string output = oss.str();
	return write(fd, output.c_str(), output.length());
}


// Startup sequence requires throttling down to safe margin
// to allow arming the drone
bool DroneState::startup(int serial_port) {
	// NOTE: hardcoded roll trim value for current test drone
	if (this->send(serial_port, this->settrim(0, 64, -800, 0)) < 0) {
		std::cerr << "Failed to send throttle down signal" << std::endl;
		return false;
	}
	sleep(1);
	if (this->send(serial_port, this->arm()) < 0) {
		std::cerr << "Failed to send arming signal" << std::endl;
		return false;
	}
	sleep(1);
	if (this->send(serial_port, this->settrim(0, 64, 0, 0)) < 0) {
		std::cerr << "Failed to reset throttle" << std::endl;
		return false;
	}
	return true;
}


std::string DroneState::arm() {
	this->armed = true;
	return std::string("{'armed':true}");
}


std::string DroneState::disarm() {
	this->armed = false;
	return std::string("{'armed':false}");
}


std::string DroneState::setpoint(float x, float y, float z) {
	std::stringstream ss;
	ss << "[" << x << "," << y << "," << z << "]";
	return std::string("{'setpoint':[" + ss.str() + "]}");
}


std::string DroneState::setpos(float x, float y, float z, float yaw) {
	position.x = x;
	position.y = y;
	position.z = z;
	position.yaw = yaw;
	std::stringstream ss;
	ss << "[" << x << "," << y << "," << z << "," << yaw << "]";
	return std::string("{'pos':" + ss.str() + "}");
}


std::string DroneState::settrim(float x, float y, float z, float yaw) {
	std::stringstream ss;
	ss << "[" << x << "," << y << "," << z << "," << yaw << "]";
	return std::string("{'trim':" + ss.str() + "}");
}


std::string DroneState::adjustpos(std::string var, std::string change) {
	if (var == "x") {
		if (change == "+") 
			position.x += INCREMENTATION;
		if (change == "-") 
			position.x -= INCREMENTATION;
	}
	if (var == "y") {
		if (change == "+") 
			position.y += INCREMENTATION;
		if (change == "-") 
			position.y -= INCREMENTATION;
	}
	if (var == "z") {
		if (change == "+") 
		position.z += INCREMENTATION;
		if (change == "-") 
			position.z -= INCREMENTATION;
	}
	if (var == "yaw") {
		if (change == "+") 
			position.yaw += INCREMENTATION;
		if (change == "-") 
			position.yaw -= INCREMENTATION;
	}
	std::stringstream ss;
	ss << "[" << position.x << "," << position.y << "," << position.z << "," << position.yaw << "]";
	return std::string("{'pos':" + ss.str() + "}");
}


std::string DroneState::adjusttrim(std::string var, std::string change) {
	if (var == "x") {
		if (change == "+") 
			trim.x -= TRIM_INCREMENTATION;
		if (change == "-") 
			trim.x += TRIM_INCREMENTATION;
	}
	if (var == "y") {
		if (change == "+") 
			trim.y -= TRIM_INCREMENTATION;
		if (change == "-") 
			trim.y += TRIM_INCREMENTATION;
	}
	if (var == "z") {
		if (change == "+") 
			trim.z -= TRIM_INCREMENTATION;
		if (change == "-") 
			trim.z += TRIM_INCREMENTATION;
	}
	if (var == "yaw") {
		if (change == "+") 
			trim.yaw -= TRIM_INCREMENTATION;
		if (change == "-") 
			trim.yaw += TRIM_INCREMENTATION;
	}
	std::stringstream ss;
	ss << "[" << trim.x << "," << trim.y << "," << trim.z << "," << trim.yaw << "]";
	return std::string("{'trim':" + ss.str() + "}");
}


std::string DroneState::adjustlight(std::string var, std::string change) {
	std::stringstream ss;
	if (var == "angle") {
		if (change == "+") 
			light.angle += 5;
		if (change == "-") 
			light.angle -= 5;
	}
	if (var == "power") {
		if (change == "+") 
			light.power += 5;
		if (change == "-") 
			light.power -= 5;
	}
	ss << "[" << light.angle << "," << light.power << "]";
	return std::string("{'light':" + ss.str() + "}");
}
