#include "Path.hpp"

// Path::~Path() {}

Path::Path(std::string file_path, DroneState & ref) : drone(ref)
{
	std::ifstream inputFile(file_path);
	std::string str((std::istreambuf_iterator<char>(inputFile)),
						std::istreambuf_iterator<char>());
	jsonStr = str;

	if (jsonStr.empty()) {
		std::cerr << "Error: JSON input is empty!" << std::endl;
		return;
	}

	// Parse frames
	try {
		json data = json::parse(jsonStr);
		this->name = data["path_name"];
		this->lenght = data["length"];
		this->fps = data["fps"];
		for (const auto& frame : data["frames"]) {
			FrameData f;
			f.frame = frame["frame"];
			f.location = {
				frame["location"]["x"],
				frame["location"]["y"],
				frame["location"]["z"]
			};
			f.rotation = {
				frame["rotation"]["x"],
				frame["rotation"]["y"],
				frame["rotation"]["z"]
			};
			f.light = {
				frame["light"]["power"],
				frame["light"]["angle"]
			};
			frames.push_back(f);
		}
	} catch (nlohmann::json::parse_error& e) {
		std::cerr << "Parse error: " << e.what() << std::endl;
	}
	std::cout << "JSON Parsing sucesfull!" << std::endl;

}

std::vector<FrameData> & Path::getFrames() {
	return frames;
}

int Path::getLenght() {
	return lenght;
}

int Path::sendFrameByFrame(crow::websocket::connection*& wsConn, std::mutex & wsMutex) {
	
	std::cout << "about the send frames" << std::endl;
	std::thread sendAllFrames ([this, wsConn, &wsMutex]() { // threading this part so it doesnt block other operations
		std::vector<FrameData>& frames = getFrames();
		for (size_t i = 0; i < lenght; i++) {
			
			std::cout << "about the send frame " << i << std::endl;
			std::stringstream ssSerial;
			ssSerial << "{" 
					<< "\"setpoint\":[" << frames[i].location.x << "," << frames[i].location.y << "," << frames[i].location.z << "],"
					<< "\"light\":[" << frames[i].light.angle << "," << frames[i].light.power << "]"
					<< "}";
			{
				std::lock_guard<std::mutex> guardSerial(drone.serialMutex);
				drone.send(ssSerial.str().c_str());
			}
			std::stringstream ssfront;
			ssfront << drone.index << ssSerial.str();
			{
				std::lock_guard<std::mutex> guard(wsMutex);
				if (wsConn)
					wsConn->send_text(ssfront.str().c_str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	});
	sendAllFrames.join();

	return 0;
}

Path::Path(Path const &cpy) : drone(cpy.drone){
	(void)cpy;
	// TODO ?
}

Path & Path::operator=(Path const &rhs) {
	(void)rhs;
	return *this;
	//TODO ?
}





