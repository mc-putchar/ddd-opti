#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <mutex>
#include <atomic>
#include "json.hpp"

#include <thread>  // For std::this_thread::sleep_for
#include <chrono>  // For std::chrono::seconds

#include "DroneState.hpp"

using json = nlohmann::json;

class DroneState;

struct Location {
	float x, y, z, yaw;
};

struct Light {
	float angle, power;
};

struct FrameData {
	int frame;
	Location location;
	Light light;
};

class Path { 

public:
	Path(std::string file, DroneState & drone);
	Path() = delete;
	Path(Path const &cpy);
	Path &operator=(Path const &rhs);
	~Path();

	int							getLenght();
	int							sendFrameByFrame();
	std::stringstream 			getCurrentFrame(size_t frame);

	DroneState &			drone;
	std::atomic<bool>		paused;
	size_t					currframe;
	bool					sending;
	size_t					length;

private:
	int						fps;
	std::string 			name;
	std::vector<FrameData>	frames;
	std::string				jsonStr;

};

