#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include "DroneState.hpp"


DroneState::DroneState(int idx, SerialHandler & ref)
	: path(NULL),
		index(idx),
		armed(false),
		position{0, 0, 0, 0},
		trim{0, 0, 0, 0},
		light{0, 0},
		serialHandler(ref),
		lastTimestamp(std::chrono::steady_clock::now())
		{}

DroneState::DroneState(DroneState const &cpy)
	: path(cpy.path),
	  index(cpy.index),
	  armed(cpy.armed.load()),
	  position(cpy.position),
	  trim(cpy.trim),
	  light(cpy.light),
	  serialHandler(cpy.serialHandler)
	{ }



DroneState::~DroneState() {
	std::cout << "drone " << index << " went out of scope" << std::endl;
}

void DroneState::setPath(Path * p) {
	path = p;
}


bool DroneState::is_armed() const { return this->armed; }


ssize_t DroneState::send(std::string const &msg) {

	// TODO imPLEMENT DEKAY COUNTER OF 2 SEC
	std::ostringstream oss;
	oss << this->index << "{" << msg << "}";  // Append index and message in a single step.
	std::string output = oss.str();
	{
		std::lock_guard<std::mutex> guard(timestampMutex);
		this->lastTimestamp = std::chrono::steady_clock::now();
	}
	return (serialHandler.send(output));
}


// Startup sequence requires throttling down to safe margin
// to allow arming the drone
bool DroneState::startup() {
    bool success = true;  // Variable to track success

    std::thread sendAllFrames([this, &success]() {
        int a, b, c;
        a = this->send(this->settrim(0, 64, -800, 0));
        sleep(1);
        b = this->send(this->arm());
        sleep(1);
        c = this->send(this->settrim(0, 64, 0, 0));
        
        if (a == 0 || b == 0 || c == 0) {
            std::cerr << "Failed to send startup" << std::endl;
            success = false;  // Set success to false if any send fails
        }
    });
    sendAllFrames.join();  // Ensure the thread completes before returning

	// MOKCUP POSIOTN DATA LOAD
	// std::thread updatepos([this]() {
	// 	float a = 3.6;
	// 	float b = 3.2;
	// 	float c = 4.5;
	// 	float d = 4.5;
	// 	while(1) {
			
	// 		std::this_thread::sleep_for(std::chrono::milliseconds(9)); // 120 fps
	// 		std::stringstream ss;
	// 	ss << "\"pos\":[" << a << "," << b << "," 
	// 				<< c << "," << d << "]";
	// 		send(ss.str().c_str());
	// 	}
	// });
	// updatepos.detach(); // Detach the thread

    return success;  // Return the result of the thread operation
}



std::string DroneState::arm() {
	armed.store(true);
	keepAlive();	
	return std::string("\"armed\":true");
}


void DroneState::disarm() {
	armed.store(false);
	// stopKeepAlive();
	this->send("\"armed\":false");
}


std::string DroneState::setpoint(float x, float y, float z, float yaw) {
	std::stringstream ss;
	(void)yaw;
	ss << "[" << x << "," << y << "," << z << "]";
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
	return std::string("\"pos\":" + ss.str());
}


std::string DroneState::settrim(int16_t x, int16_t y, int16_t z, int16_t yaw) {
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


std::string DroneState::setlight(int angle, int power) {
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
			std::this_thread::sleep_for(std::chrono::milliseconds(1500)); // Check every 1.5 seconds
			auto now = std::chrono::steady_clock::now();
			std::chrono::steady_clock::time_point copylastTimestamp;
			std::chrono::duration<double> elapsed;
			{
				std::lock_guard<std::mutex> guard(timestampMutex);
				copylastTimestamp = lastTimestamp;
				elapsed = now - copylastTimestamp; // Calculate elapsed time.
			}
			if (elapsed.count() > 1.5) { // More than 1.5 seconds have passed
				send("\"ping\":true");
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