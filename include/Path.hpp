#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"

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

private:
	Path(){}
	// int						lenght;
	// std::string 			name;
	std::vector<FrameData>	frames;


};

