#include <iostream>

#include "json.hpp"

#include "DroneControl.hpp"

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
	std::cout << "Tracking drone id: " << this->drones.size() - 1 << std::endl;
}

void DroneControl::update(int idx, json const &data)
{
	if (data.contains("trim")) {
		drones[idx]->send(drones[idx]->settrim(
			data["trim"][0], data["trim"][1], data["trim"][2], data["trim"][3]));
	}

	if (data.contains("light")) {
		drones[idx]->send(drones[idx]->setlight(data["light"][0], data["light"][1]));
	}

	if (data.contains("setpoint")) {
		drones[idx]->send(drones[idx]->setpoint(data["setpoint"][0], data["setpoint"][1],
													data["setpoint"][2], data["setpoint"][3]));
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
				std::cerr << "Error: Path is null for drone " << idx << std::endl;
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
			std::cerr << "Error: Path is null for drone " << idx << std::endl;
		}
	}
}

void DroneControl::setPath(int idx, Path *path)
{
	this->drones[idx]->setPath(path);
}
