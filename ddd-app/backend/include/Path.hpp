#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"
#include "crow_all.h"

#include <thread>  // For std::this_thread::sleep_for
#include <chrono>  // For std::chrono::seconds
#include <unistd.h>

using json = nlohmann::json;

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
	Path(std::string file);
	Path(Path const &cpy);
	Path &operator=(Path const &rhs);
	~Path();

	int send(crow::websocket::connection*& wsConn);

private:
	size_t					lenght;
	int						fps;
	std::string 			name;
	std::vector<FrameData>	frames;
	std::string				jsonStr;

	Path(){}


};

