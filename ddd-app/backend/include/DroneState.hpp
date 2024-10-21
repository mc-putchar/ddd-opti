#ifndef DRONESTATE_HPP
#define DRONESTATE_HPP

#include <map>
#include <sstream>
#include <string>
#include <mutex>

#include "Path.hpp"

class Path;

struct SetPoint {
	float x, y, z;
	float yaw, pitch, roll;
	SetPoint(float x, float y, float z, float yaw, float pitch, float roll)
			: x(x), y(y), z(z), yaw(yaw), pitch(pitch), roll(roll) {}
	SetPoint(std::string setpoint) {
		std::istringstream iss(setpoint);
		iss >> this->x;
		iss >> this->y;
		iss >> this->z;
	}
};

struct Position {
	float x, y, z, yaw;
};

struct Trim {
	float x, y, z, yaw;
};

struct DLight {
	float power, angle;
};

// NOTE: possible keys:
// armed = bool
// pos = array[4]
// vel = array[3]
// setpoint = array[3]
// pid = array[17]
// trim = array[4]
// light = var;
// servo = var;
class DroneState {
public:
	DroneState() = delete;
	DroneState(int idx, int serialport, std::mutex & ref);
	DroneState(DroneState const &cpy);
	DroneState &operator=(DroneState const &rhs);
	~DroneState();

	bool is_armed() const;

	ssize_t send(std::string const &msg);

	bool startup();
	std::string arm();
	std::string disarm();
	std::string setpoint(float x, float y, float z, float yaw);
	std::string setpos(float x, float y, float z, float yaw);
	std::string settrim(float x, float y, float z, float yaw);
	std::string adjustpos(std::string var, std::string change);
	std::string adjusttrim(std::string var, std::string change);
	std::string adjustlight(std::string var, std::string change);
	void		setPath(std::unique_ptr<Path> p);

	std::unique_ptr<Path>	path; 
	std::mutex &			serialMutex;
	const int				index;

private:
	const int 				serialPort;
	bool					armed;
	Position				position;
	Trim					trim;
	DLight					light;
	std::mutex				droneDataMutex; //To protect the read and write of the 3 struct.
};

#endif // DRONESTATE_HPP
