// #include <iostream>
#include <cstdint>
#include <string>

#include "json.hpp"

#include "DroneControl.hpp"
#include "Log.hpp"

#define TAG	"DroneCtrl"

DroneControl &DroneControl::getInstance()
{
	static DroneControl instance;
	return instance;
}

DroneControl::DroneControl()
	: drones() //, msgQue()
{}

DroneControl::~DroneControl()
{}

void DroneControl::track(std::shared_ptr<DroneState> drone)
{
	this->drones.push_back(drone);
	std::string tmp = "Tracking drone id: " + std::to_string(this->drones.size() - 1);
	INFO(TAG, tmp.c_str());
}

#include <cmath>
static inline float quaternion_to_yaw(float x, float y, float z, float w)
{
	float sycp = (x * y + z * w) * 2.0f;
	float cycp = 1 - (y * y + z * z) * 2.0f;
	return (std::atan2f(sycp, cycp));
}

void DroneControl::update(int idx, float position[3], float quaternions[4])
{
	float pos[4];

	pos[0] = position[0];
	pos[1] = position[1];
	pos[2] = position[2];
	pos[3] = quaternion_to_yaw(quaternions[0], quaternions[1], quaternions[2], quaternions[3]);
	// TODO: Sync updates to clock
	drones[idx]->update_position(pos);
}

void DroneControl::update(int idx, json const &data)
{
	if (data.contains("trim")) {
		drones[idx]->send(drones[idx]->settrim(
			data["trim"][0],
			data["trim"][1],
			data["trim"][2],
			data["trim"][3]
		));
	}

	if (data.contains("light")) {
		drones[idx]->send(drones[idx]->setlight(
			data["light"][0],
			data["light"][1]
		));
	}

	if (data.contains("setpoint")) {
		drones[idx]->send(drones[idx]->setpoint(
			data["setpoint"][0],
			data["setpoint"][1],
			data["setpoint"][2],
			data["setpoint"][3]
		));
	}

	if (data.contains("armed")) {
		if (data["armed"].get<bool>()) 
			drones[idx]->startup();
		else
			drones[idx]->disarm();
	}

	if (data.contains("path")) {
		if (data["path"] == "play") {
			// Check if the unique pointer is not null
			if (drones[idx]->path != nullptr) {
				drones[idx]->path->sendFrameByFrame();
			} else {
				std::string tmp = "Path is null for drone " + std::to_string(idx);
				ERROR(TAG, tmp.c_str());
			}
		}
		if (data["path"] == "pause") {
			drones[idx]->path->paused.store(true);
		}
		if (data["path"] == "stop") {
			drones[idx]->path->sending=false;
		}
	}
	if (data.contains("frame")) {
		if (drones[idx]->path != nullptr) {
			drones[idx]->send(drones[idx]->path->getCurrentFrame(data["frame"]).str().c_str());
		} else {
			std::string tmp = "Path is null for drone " + std::to_string(idx);
			ERROR(TAG, tmp.c_str());
		}
	}
}

void DroneControl::setPath(int idx, DronePath *path)
{
	this->drones[idx]->setPath(path);
}
