#include "../include/InputOptiMock.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath> 



InputOptiMock::InputOptiMock() : rbIndex(0), Sx(1.0f), Sy(1.0f), Sz(1.0f),
		  x(0.0f), y(0.0f), z(0.0f),
          qx(0.0f), qy(0.0f), qz(0.0f), qw(0.0f),
          yaw(0.0f), pitch(0.0f), roll(0.0f)
{
	int value;
	while (true) {

		const float frequency = 2 * M_PI / TIME_TO_SETPOINT;
        // Print value and move cursor back to the start of the line
		clearScreen();
        std::cout << "Setpoint  X: " << Sx << std::endl;
		std::cout << "Current   X: " << x  << std::endl;
		printBar(x, "            "); // Print bar for X
		std::cout << "Setpoint  Y: " << Sx << std::endl;
		std::cout << "Current   Y: " << x  << std::endl;
		printBar(y, "            "); // Print bar for Y
		std::cout << "Setpoint  Z: " << Sx << std::endl;
		std::cout << "Current   Z: " << x  << std::endl;
		printBar(z, "            "); // Print bar for Z


		// Update x to oscillate between 0 and 1
        // Calculate the time in seconds since the start of the program
        static auto startTime = std::chrono::steady_clock::now();
        auto currentTime = std::chrono::steady_clock::now();
        float elapsedSeconds = std::chrono::duration<float>(currentTime - startTime).count();

        // Calculate x using sine wave oscillation
        x = (std::sin(frequency * elapsedSeconds) + 1) / 2;

        // Increment value
		// y += 0.01;
		// z += 0.01;

        // Wait for 100 milliseconds
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void InputOptiMock::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void InputOptiMock::printBar(float value, const std::string& label) {
    // Calculate the number of characters to represent the value
    int barPosition = static_cast<int>(value * BAR_LENGTH);
    
    std::cout << label << " |";
    for (int i = 0; i < BAR_LENGTH; ++i) {
        if (i < barPosition) {
            std::cout << " ";
        } else if (i == barPosition) {
            std::cout << "-"; // Mark the current position
        } else {
            std::cout << " ";
        }
    }
    std::cout << "|\n\n" << std::endl;
}


InputOptiMock::~InputOptiMock()
{

}
