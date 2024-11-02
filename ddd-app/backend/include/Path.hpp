#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <mutex>
#include "json.hpp"
#include "crow_all.h"

#include <thread>  // For std::this_thread::sleep_for
#include <chrono>  // For std::chrono::seconds
#include <unistd.h>

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

	std::vector<FrameData> &	getFrames();
	int							getLenght();
	int							sendFrameByFrame();
	DroneState &			drone;

private:
	size_t					length;
	int						fps;
	std::string 			name;
	std::vector<FrameData>	frames;
	std::string				jsonStr;
	bool					sending;

};

