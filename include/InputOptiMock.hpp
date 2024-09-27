#pragma once
#include <iostream>
#define BAR_LENGTH 30
#define TIME_TO_SETPOINT 1.8 // Setpoint oscillation period in seconds

class InputOptiMock
{
	private:

	int rbIndex;
	float x;
	float y; 
	float z;
	float Sx;
	float Sy; 
	float Sz;
	float qx;
	float qy;
	float qz;
	float qw;
	float yaw;
	float pitch;
	float roll;

	public:

	InputOptiMock();
	~InputOptiMock();

	void clearScreen();
	void printBar(float value, const std::string& label);






	
	




};