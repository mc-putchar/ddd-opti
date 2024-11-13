#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "json.hpp"

#include <DroneState.hpp>

using json = nlohmann::json;

class DroneControl {
	public:
		static DroneControl &getInstance();
		DroneControl(DroneControl const &) = delete;
		DroneControl(DroneControl const &&) = delete;
		void operator=(DroneControl const &) = delete;
		void operator=(DroneControl const &&) = delete;
		~DroneControl();

		void track(std::shared_ptr<DroneState> drone);
		void update(int idx, json const &data);
		void setPath(int idx, Path *path);

	private:
		DroneControl();

		std::vector<std::shared_ptr<DroneState>> drones;
		// std::queue<std::string> msgQue;
};
