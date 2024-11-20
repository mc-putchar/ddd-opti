#pragma once

#include <vector>

#include "json.hpp"

#include <DroneState.hpp>

using json = nlohmann::json;

class DroneControl {
	public:
		static DroneControl &getInstance(void);
		DroneControl(DroneControl const &) = delete;
		DroneControl(DroneControl const &&) = delete;
		void operator=(DroneControl const &) = delete;
		void operator=(DroneControl const &&) = delete;
		~DroneControl();

		void track(std::shared_ptr<DroneState> drone);
		void update(int idx, json const &data);
		void setPath(int idx, DronePath *path);

	private:
		DroneControl();

		std::vector<std::shared_ptr<DroneState>> drones;
		// std::queue<std::string> msgQue;
};
