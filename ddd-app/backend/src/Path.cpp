#include "Path.hpp"

Path::Path(std::string file_path) {

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

	// Output for checking
	// for (const auto& f : frames) {
	// 	std::cout << "Frame: " << f.frame << "\n"
	// 			  << "Location: " << f.location.x << ", " << f.location.y << ", " << f.location.z << "\n"
	// 			  << "Rotation: " << f.rotation.x << ", " << f.rotation.y << ", " << f.rotation.z << "\n"
	// 			  << "Light Power: " << f.light.power << ", Angle: " << f.light.angle << "\n\n";
	// }
}

int Path::send(crow::websocket::connection*& wsConn) {
	
	for (size_t i = 0; i < lenght; i++)
	{
		std::stringstream ss;

		ss << "Frame: " << frames[i].frame << "<br />"
				  << "Location: " << frames[i].location.x << ", " << frames[i].location.y << ", " << frames[i].location.z << "<br />"
				  << "Rotation: " << frames[i].rotation.x << ", " << frames[i].rotation.y << ", " << frames[i].rotation.z << "<br />"
				  << "Light Power: " << frames[i].light.power << ", Angle: " << frames[i].light.angle << "<br /><br />";

		if (wsConn)
			wsConn->send_text(ss.str().c_str());
		sleep(1); // std::this_thread::sleep_for(std::chrono::seconds(1)); this is better
	}
	return 0;
}

Path::Path(Path const &cpy) {
	(void)cpy;
	// TODO ?
}

Path & Path::operator=(Path const &rhs) {
	(void)rhs;
	return *this;
	//TODO ?
}

Path::~Path() {
}




