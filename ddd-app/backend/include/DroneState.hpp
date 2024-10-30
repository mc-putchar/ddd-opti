#ifndef DRONESTATE_HPP
#define DRONESTATE_HPP

#include <map>
#include <sstream>
#include <string>
#include <mutex>
#include <chrono>

#include "Path.hpp"
#include "SerialHandler.hpp"

class Path;
class SerialHandler;

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
	DroneState(int idx, SerialHandler & ref);
	DroneState(DroneState const &cpy);
	DroneState &operator=(DroneState const &rhs);
	~DroneState();

	bool is_armed() const;

	ssize_t send(std::string const &msg);

	bool		startup();
	int			sendAll();
	std::string	arm();
	void		disarm();
	std::string setpoint(float x, float y, float z, float yaw);
	std::string setpos(float x, float y, float z, float yaw);
	std::string settrim(float x, float y, float z, float yaw);
	std::string adjustpos(std::string var, std::string change);
	std::string adjusttrim(std::string var, std::string change);
	std::string setlight(float angle, float power);
	void		setPath(std::unique_ptr<Path> p);
	void keepAlive();

	std::unique_ptr<Path>	path; 
	const int				index;

private:
	bool					armed;
	Position				position;
	Trim					trim;
	DLight					light;
	SerialHandler &			serialHandler;
	std::mutex				droneDataMutex; //To protect the read and write of the 3 struct.
	std::chrono::steady_clock::time_point lastTimestamp;
};

#endif // DRONESTATE_HPP
