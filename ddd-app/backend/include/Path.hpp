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
	float x, y, z;
};

struct Rotation {
	float x, y, z;
};

struct Light {
	float power, angle;
};

struct FrameData {
	int frame;
	Location location;
	Rotation rotation;
	Light light;
};

class Path {
public:
	Path(std::string file, DroneState & drone);
	Path() = delete;
	Path(Path const &cpy);
	Path &operator=(Path const &rhs);
	~Path(){}

	std::vector<FrameData> &	getFrames();
	int							getLenght();
	int							sendFrameByFrame(crow::websocket::connection*& wsConn,
													std::mutex & wsMutex);
	DroneState &			drone;

private:
	size_t					lenght;
	int						fps;
	std::string 			name;
	std::vector<FrameData>	frames;
	std::string				jsonStr;

};

