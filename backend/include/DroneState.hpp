#ifndef DRONESTATE_HPP
#define DRONESTATE_HPP

#include <map>
#include <sstream>
#include <string>
#include <mutex>
#include <chrono>

#include "DronePath.hpp"
#include "SerialHandler.hpp"

class DronePath;
class SerialHandler;

struct SetPoint {
	float x, y, z;
	SetPoint(float x, float y, float z)
			: x(x), y(y), z(z) {};
	SetPoint(const SetPoint& cpy)
			: x(cpy.x), y(cpy.y), z(cpy.z) {}
	SetPoint& operator=(SetPoint const &rhs);
	SetPoint(std::string setpoint) {
		std::istringstream iss(setpoint);
		iss >> this->x;
		iss >> this->y;
		iss >> this->z;
	}
};

struct Position {
	float x, y, z, yaw;
	Position(Position const &cpy) = default;
	Position &operator=(Position const &rhs) = default;
	Position &operator=(float pos[4]) {
		this->x = pos[0];
		this->y = pos[1];
		this->z = pos[2];
		this->yaw = pos[3];
		return *this;
	};
};

struct Trim {
	int x, y, z, yaw;
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
	DroneState(int idx);
	DroneState(DroneState const &cpy);
	~DroneState();

	bool is_armed() const;

	ssize_t send(std::string const &msg);

	bool		startup();
	int			sendAll();
	std::string	arm();
	void		disarm();
	std::string setpoint(float x, float y, float z);
	std::string setpos(float x, float y, float z, float yaw);
	std::string settrim(float x, float y, float z, float yaw);
	std::string setlight(float angle, float power);
	void		setPath(DronePath *p);
	void		keepAlive();
	void		stopKeepAlive();
	void		update_position(float pos[4]);

	DronePath				*path; 
	const int				index;

private:
	std::atomic<bool>		armed; // Use atomic for thread-safe access
	Position				position;
	Trim					trim;
	DLight					light;
	SetPoint				target;
	std::mutex				droneDataMutex; //To protect the read and write of the 3 struct.
	std::mutex				timestampMutex;
	std::thread				keepAliveThread;
	std::chrono::steady_clock::time_point lastTimestamp;
};

#endif // DRONESTATE_HPP
