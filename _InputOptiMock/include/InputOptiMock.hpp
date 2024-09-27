#pragma once
#include <iostream>
#define BAR_LENGTH 30
#define TIME_TO_SETPOINT 1.8 // Setpoint oscillation period in seconds

class InputOptiMock
{
	private:

	int rbIndex;
    float Sx, Sy, Sz;
    float x, y, z; 
    float qx, qy, qz, qw;
    float yaw, pitch, roll;

	public:

	InputOptiMock();
	~InputOptiMock();

	void clearScreen();
	void printBar(float value, const std::string& label);






	
	




};