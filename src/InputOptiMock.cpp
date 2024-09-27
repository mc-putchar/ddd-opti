#include "../include/InputOptiMock.hpp"
#include <iostream>
#include <chrono>
#include <thread>



InputOptiMock::InputOptiMock() : rbIndex(0), Sx(1.0f), Sy(1.0f), Sz(1.0f),
		  x(0.0f), y(0.0f), z(0.0f),
          qx(0.0f), qy(0.0f), qz(0.0f), qw(0.0f),
          yaw(0.0f), pitch(0.0f), roll(0.0f)
{
	int value;
	while (true) {
        // Print value and move cursor back to the start of the line
		clearScreen();
        std::cout << "\rSetpoint  X: " << Sx << std::endl;
		std::cout << "\rCurrent   X: " << x  << std::endl;
		std::cout << "\rSetpoint      Y: " << Sy << std::endl;
		std::cout << "\rCurrent       Y: " << y << std::endl;
		std::cout << "\rSetpoint             Z:" << Sz << std::endl;
		std::cout << "\rCurrent              Z:" << z << std::endl;
        // Increment value
        x += 0.01;
		y += 0.01;
		z += 0.01;


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


InputOptiMock::~InputOptiMock()
{

}
