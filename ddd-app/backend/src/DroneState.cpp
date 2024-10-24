#include <iostream>
#include <sstream>
#include <string>

#include <unistd.h>

#include "DroneState.hpp"

#define INCREMENTATION 0.3
#define TRIM_INCREMENTATION 25

// DroneState::DroneState() 
// 	: path(nullptr),
// 		index(0),
// 		serialPort(0),
// 		armed(false),
// 		position{0, 0, 0, 0},
// 		trim{0, 0, 0, 0},
// 		light{0, 0}
// 		{}


DroneState::DroneState(int idx, SerialHandler & ref)
	: path(nullptr),
		index(idx),
		armed(false),
		position{0, 0, 0, 0},
		trim{0, 0, 0, 0},
		light{0, 0},
		serialHandler(ref)
		{}


DroneState::DroneState(DroneState const &cpy)
	: path(nullptr),
		index(cpy.index),
		armed(cpy.armed),
		position{cpy.position.x, cpy.position.y, cpy.position.z, cpy.position.yaw},
		trim{cpy.trim.x, cpy.trim.y, cpy.trim.z, cpy.trim.yaw},
		light{cpy.light.power, cpy.light.angle},
		serialHandler(cpy.serialHandler)
		{}


DroneState &DroneState::operator=(DroneState const &rhs) {
	if (this == &rhs)
		return *this;
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


ssize_t DroneState::send(std::string const &msg) {

	// TODO imPLEMENT DEKAY COUNTER OF 2 SEC

	std::ostringstream oss;
	oss << this->index << "{" << msg << "}";  // Append index and message in a single step.
	std::string output = oss.str();
	
	return (serialHandler.send(output));
}


// Startup sequence requires throttling down to safe margin
// to allow arming the drone
bool DroneState::startup() {
	// NOTE: hardcoded roll trim value for current test drone
	if (this->send(this->settrim(0, 64, -800, 0)) < 0) {
		std::cerr << "Failed to send throttle down signal" << std::endl;
		return false;
	}
	sleep(1);
	if (this->send(this->arm()) < 0) {
		std::cerr << "Failed to send arming signal" << std::endl;
		return false;
	}
	sleep(1);
	if (this->send(this->settrim(0, 64, 0, 0)) < 0) {
		std::cerr << "Failed to reset throttle" << std::endl;
		return false;
	}
	return true;
}


std::string DroneState::arm() {
	std::lock_guard<std::mutex> guard(droneDataMutex);
	this->armed = true;
	return std::string("\"armed\":true");
}


void DroneState::disarm() {
	std::lock_guard<std::mutex> guard(droneDataMutex);
	this->armed = false;
	this->send("\"armed\":false");
}


std::string DroneState::setpoint(float x, float y, float z, float yaw) {
	std::stringstream ss;
	(void)yaw;
	ss << "[" << x << "," << y << "," << z << "]";
	return std::string("\"setpoint\":" + ss.str());
}


std::string DroneState::setpos(float x, float y, float z, float yaw) {
	std::lock_guard<std::mutex> guard(droneDataMutex);
	position.x = x;
	position.y = y;
	position.z = z;
	position.yaw = yaw;
	std::stringstream ss;
	ss << "[" << x << "," << y << "," << z << "," << yaw << "]";
	return std::string("\"pos\":" + ss.str());
}


std::string DroneState::settrim(float x, float y, float z, float yaw) {
	std::stringstream ss;
	ss << "[" << x << "," << y << "," << z << "," << yaw << "]";
	return std::string("\"trim\":" + ss.str());
}


std::string DroneState::setlight(float angle, float power) {
	std::lock_guard<std::mutex> guard(droneDataMutex);
	std::stringstream ss;

	light.angle = angle;
	light.power = power;
	ss << "[" << light.angle << "," << light.power << "]";
	return std::string("\"light\":" + ss.str());
}

int DroneState::sendAll() {
	std::stringstream ss;

	ss << "\"armed\":" << armed << "," 
		<< setpos(position.x, position.y, position.z, position.yaw) << ","
		<< settrim(trim.x, trim.y, trim.z, trim.yaw) << ","
		<< setlight(light.angle, light.power);

	return (this->send(ss.str()));
}


		// auto currentTime = std::chrono::steady_clock::now();
		// if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastSendTime).count() >= MIN_INTER_SEND) {
		// 	const char* ping = "0{\"ping\"}"; // HOW WILL WE HANDLE MULTIPLE PING DRONES MONITORING?
		// 	wb = write(serial_port, ping, strlen(ping));
		// 	if (wb < 0) {
		// 		std::cerr << "Failed to send serial data to transmitter." << std::endl;
		// 	} else {
		// 		std::cout << "Pinged the receiver" << std::endl;
		// 		send_to_ws(ping);
		// 	}
		// 	lastSendTime = currentTime; // Update the last send time
		// }