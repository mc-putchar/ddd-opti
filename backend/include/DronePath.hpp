#pragma once

#include <atomic>
#include <string>
#include <vector>
#include <fstream>
// #include <iostream>
#include <mutex>

#include <chrono>  // For std::chrono::seconds
#include <thread>  // For std::this_thread::sleep_for

#include "json.hpp"

using json = nlohmann::json;

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

class DronePath { 

public:
	DronePath(std::string file);
	DronePath() = delete;
	DronePath(DronePath const &cpy);
	DronePath &operator=(DronePath const &rhs);
	~DronePath();

	int							getLenght();
	int							sendFrameByFrame();
	std::stringstream 			getCurrentFrame(size_t frame);

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

