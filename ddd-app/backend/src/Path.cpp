#include "Path.hpp"


Path::Path(std::string file_path, DroneState & ref) : drone(ref), sending(false)
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
		this->length = data["length"];
		this->fps = data["fps"];
		currframe = 0;
		for (const auto& frame : data["frames"]) {
			FrameData f;
			f.frame = frame["frame"];
			f.location = {
				frame["location"]["x"],
				frame["location"]["y"],
				frame["location"]["z"],
				frame["rotation"]["z"] // TODO ADD THE YAW TRANSFOMATION MAGIC FORMULA
			};
			float power = frame["light"]["power"];
			f.light = {
				frame["light"]["angle"],
				static_cast<float>(static_cast<int>(power) / 10)
			};
			frames.push_back(f);
		}
	} catch (nlohmann::json::parse_error& e) {
		std::cerr << "Parse error: " << e.what() << std::endl;
	}
}

std::stringstream Path::getCurrentFrame(size_t i) {
	std::stringstream ss;
			ss << "\"setpoint\":[" << frames[i].location.x << "," << frames[i].location.y << "," << frames[i].location.z << "," << frames[i].location.yaw << "],"
					  << "\"light\":[" << frames[i].light.angle << "," << frames[i].light.power << "]" << ","
					<< "\"frame\":" << i;
	return (ss);
}

int Path::sendFrameByFrame() {
	if (sending) {
		std::cout << "sending already" << std::endl;
		if (paused.load() == true)
			paused.store(false);
		return 0;
	}
	sending = true;

	// Create a new thread for sending frames
	 std::thread sendAllFrames([this]() {
		paused.store(false);
		for (currframe = 0; currframe < length; currframe++) {
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			if (!sending) break;
			while (paused.load() == true) {
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
			std::cout << "about the send frame " << currframe << std::endl;
			std::stringstream ssframe = getCurrentFrame(currframe);
			if (drone.send(ssframe.str().c_str()) < 0)
				std::cout << "serial send failed " << currframe << std::endl;
		}
		sending = false;
		});
	sendAllFrames.detach(); // Detach the thread
	return 0;
}

Path::~Path() {
	std::cout << "path " << " went out of scope" << std::endl;
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





