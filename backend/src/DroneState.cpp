// #include <iostream>
#include <cstdint>
#include <sstream>
#include <string>
#include <unistd.h>

#include "DroneState.hpp"
#include "Log.hpp"

#define TAG	"DroneState"

SetPoint& SetPoint::operator=(SetPoint const &rhs)
{
	if (this != &rhs) {
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
	}
	return *this;
}

DroneState::DroneState(int idx)
	: path(NULL),
	index(idx),
	armed(false),
	position{0, 0, 0, 0},
	trim{0, 0, 0, 0},
	light{0, 0},
	target{0, 0, 0},
	lastTimestamp(std::chrono::steady_clock::now())
{}

DroneState::DroneState(DroneState const &cpy)
	: path(cpy.path),
	index(cpy.index),
	armed(cpy.armed.load()),
	position(cpy.position),
	trim(cpy.trim),
	light(cpy.light),
	target(cpy.target)
{}



DroneState::~DroneState() {
	// std::cout << "drone " << index << " went out of scope" << std::endl;
}

void DroneState::setPath(DronePath * p) {
	path = p;
}


bool DroneState::is_armed() const { return this->armed; }


ssize_t DroneState::send(std::string const &msg) {

	std::ostringstream oss;
	oss << this->index << "{" << msg << "}";
	std::string output = oss.str();
	{
		std::lock_guard<std::mutex> guard(timestampMutex);
		this->lastTimestamp = std::chrono::steady_clock::now();
	}
	SerialHandler&	serial(SerialHandler::getInstance());
	return (serial.send(output));
}

bool DroneState::startup() {
	return (this->send(this->arm()) > 0);
}

std::string DroneState::arm() {
	armed.store(true);
	keepAlive();
	return std::string("\"armed\":true");
}


void DroneState::disarm() {
	armed.store(false);
	stopKeepAlive();
	this->send("\"armed\":false");
	// return std::string("\"armed\":true");
}

std::string DroneState::setpoint(float x, float y, float z) {
	std::stringstream ss;
	ss << "[" << x << "," << y << "," << z << "]";
	this->target = SetPoint(x, y, z);
	return std::string("\"setpoint\":" + ss.str());
}


std::string DroneState::setpos(float x, float y, float z, float yaw) {
	{
		std::lock_guard<std::mutex> guard(droneDataMutex);
		position.x = x;
		position.y = y;
		position.z = z;
		position.yaw = yaw;
	}
	std::stringstream ss;
	ss << "[" << x << "," << y << "," << z << "," << yaw << "]";
	// INFO(TAG, "Position update:");
	// INFO(TAG, ss.str().c_str());
	return std::string("\"pos\":" + ss.str());
}


std::string DroneState::settrim(float x, float y, float z, float yaw) {
	{
		std::lock_guard<std::mutex> guard(droneDataMutex);
		trim.x = x;
		trim.y = z;
		trim.z = z;
		trim.yaw = yaw;
	}
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
		<< setlight(light.angle, light.power) << ","
		<< setpoint(target.x, target.y, target.z);

	if (path){
		ss << "," << "\"length\":" << path->length << ","
		<< "\"frame\":" << path->currframe;
	}
	return (this->send(ss.str()));
}

void DroneState::keepAlive() {
	stopKeepAlive();
	keepAliveThread = std::thread([this]() {
		while (true) {
			if (!armed.load()) { // Check armed status within the thread
				break; // Exit the loop if armed is false
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			auto now = std::chrono::steady_clock::now();
			std::chrono::steady_clock::time_point copylastTimestamp;
			std::chrono::duration<double> elapsed;
			{
				std::lock_guard<std::mutex> guard(timestampMutex);
				copylastTimestamp = lastTimestamp;
				elapsed = now - copylastTimestamp; // Calculate elapsed time.
			}
			if (elapsed.count() > 1) {
				send("\"ping\":true");
				this->sendAll();
			}
		}
	});
}

void DroneState::stopKeepAlive() {
	if (keepAliveThread.joinable()) {
		armed.store(false); // Set armed to false to break the loop
		keepAliveThread.join(); // Wait for the thread to finish
	}
}

void DroneState::update_position(float pos[4])
{
	static std::uint32_t frame_counter(0);
	if (++frame_counter & 3)
		this->send(this->setpos(pos[0],pos[1],pos[2],pos[3]));
}
