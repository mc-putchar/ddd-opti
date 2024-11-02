#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include <PID_v1.h>
#include <stdint.h>
#include "sbus.h"
// #include <EEPROM.h>

#define batVoltagePin 34
#define MAX_VEL 100
#define ROTOR_RADIUS 0.0225
#define Z_GAIN 0.7
#define DRONE_INDEX 0
#define SERVOPIN 99
#define LEDPIN 26  // GPIO pin to control the LED
#define LEDPMWCHANNEL 0  // PWM channel
#define LEDPMWFREQ 5000  // Frequency of the PWM signal
#define LEDPMWRES 8
// #define EEPROM_SIZE 4

// int rep_data[5];

#if DRONE_INDEX == 0
  uint8_t mac_addr[6] = { 0x08, 0xB6, 0x1F, 0xBC, 0x8E, 0x9B };
#elif DRONE_INDEX == 1
  uint8_t mac_addr[6] = { 0x08, 0xB6, 0x1F, 0xBC, 0x8E, 0x9C };
#endif

StaticJsonDocument<1024> json;
bfs::SbusTx		sbus_tx(&Serial1, 33, 32, false, false);
bfs::SbusData	data;

bool			armed = false;
unsigned long	timeArmed = 0;
unsigned long	lastPing;
int				xTrim = 0, yTrim = 0, zTrim = 0, yawTrim = 0;
double			groundEffectCoef = 28; groundEffectOffset = -0.035;

// nested pid loops
// outer: position pid loop
// inner: velocity pid loop
// velocity pid loop sends accel setpoint to flight controller
double xPosSetpoint   = 0, xPos = 0;
double yPosSetpoint   = 0, yPos = 0;
double zPosSetpoint   = 0, zPos = 0;
double yawPosSetpoint = 0, yawPos, yawPosOutput;

double xyPosKp  = 1,   xyPosKi = 0,     xyPosKd = 0;
double zPosKp   = 1.5,  zPosKi = 0,      zPosKd = 0;
double yawPosKp = 0.3, yawPosKi = 0.1, yawPosKd = 0.05;

double xVelSetpoint, xVel, xVelOutput;
double yVelSetpoint, yVel, yVelOutput;
double zVelSetpoint, zVel, zVelOutput;

double xyVelKp = 0.2, xyVelKi = 0.03, xyVelKd = 0.05;
double zVelKp = 0.3, zVelKi = 0.1, zVelKd = 0.05;

Servo	myServo;
double	lightAngle = 0;
int		lightPower = 0;

PID xPosPID(&xPos, &xVelSetpoint, &xPosSetpoint, xyPosKp, xyPosKi, xyPosKd, DIRECT);
PID yPosPID(&yPos, &yVelSetpoint, &yPosSetpoint, xyPosKp, xyPosKi, xyPosKd, DIRECT);
PID zPosPID(&zPos, &zVelSetpoint, &zPosSetpoint, zPosKp, zPosKi, zPosKd, DIRECT);
PID yawPosPID(&yawPos, &yawPosOutput, &yawPosSetpoint, yawPosKp, yawPosKi, yawPosKd, DIRECT);

PID xVelPID(&xVel, &xVelOutput, &xVelSetpoint, xyVelKp, xyVelKi, xyVelKd, DIRECT);
PID yVelPID(&yVel, &yVelOutput, &yVelSetpoint, xyVelKp, xyVelKi, xyVelKd, DIRECT);
PID zVelPID(&zVel, &zVelOutput, &zVelSetpoint, zVelKp, zVelKi, zVelKd, DIRECT);

unsigned long	lastLoopTime = micros();
unsigned long	lastSbusSend = micros();
float			loopFrequency = 2000.0;
float			sbusFrequency = 50.0;


// Callback function that will be executed when data is received
void data_recv_cb(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
	Serial.printf("Recv %d data: ", len);
	Serial.write(incomingData, len);
	Serial.println();

	DeserializationError err = deserializeJson(json, (const char *)incomingData);
	if (err) {
		Serial.println("failed to parse json");
		return;
	}
	serializeJsonPretty(json, Serial);

	if (json.containsKey("vel") ) {
		xVel = json["vel"][0];
		yVel = json["vel"][1];
		zVel = json["vel"][2];
	}
	if (json.containsKey("pos")) {
		xPos =   json["pos"][0];
		yPos =   json["pos"][1];
		zPos =   json["pos"][2];
		yawPos = json["pos"][3];
	}
	if (json.containsKey("light")) {
		lightAngle = json["light"][0];
		lightPower = json["light"][1];
		// myServo.write(servoAngle);
		if (ledcWrite(LEDPIN, brightness)) {
			Serial.printf("LED successfully set to %d\n", brightness);
		}
	}
	if (json.containsKey("armed")) {
		if (json["armed"] != armed && json["armed"]) {
			timeArmed = millis();
		}
		armed = json["armed"];
	}
	if (json.containsKey("setpoint")) {
		xPosSetpoint = json["setpoint"][0];
		yPosSetpoint = json["setpoint"][1];
		zPosSetpoint = json["setpoint"][2];
	}
	if (json.containsKey("pid")) {
		xPosPID.SetTunings(json["pid"][0], json["pid"][1], json["pid"][2]);
		yPosPID.SetTunings(json["pid"][0], json["pid"][1], json["pid"][2]);
		zPosPID.SetTunings(json["pid"][3], json["pid"][4], json["pid"][5]);
		yawPosPID.SetTunings(json["pid"][6], json["pid"][7], json["pid"][8]);
		xVelPID.SetTunings(json["pid"][9], json["pid"][10], json["pid"][11]);
		yVelPID.SetTunings(json["pid"][9], json["pid"][10], json["pid"][11]);
		zVelPID.SetTunings(json["pid"][12], json["pid"][13], json["pid"][14]);
		groundEffectCoef = json["pid"][15];
		groundEffectOffset = json["pid"][16];
	}
	if (json.containsKey("trim")) {
		xTrim =   json["trim"][0];
		yTrim =   json["trim"][1];
		zTrim =   json["trim"][2];
		yawTrim = json["trim"][3];
	}
	lastPing = micros();
}

void resetPid(PID &pid, double min, double max) {
	pid.SetOutputLimits(0.0, 1.0); 
	pid.SetOutputLimits(-1.0, 0.0);
	pid.SetOutputLimits(min, max);
}

void readMacAddress(){
	uint8_t baseMac[6];
	esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
	if (ret == ESP_OK) {
		Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
					baseMac[0], baseMac[1], baseMac[2],
					baseMac[3], baseMac[4], baseMac[5]);
	} else {
		Serial.println("Failed to read MAC address");
	}
}

void setup() {
	Serial.begin(115200); // Initialize Serial Monitor
	
	// myServo.attach(servoPin); // servo control
	if ( ledcAttach(LEDPIN, LEDPMWFREQ, LEDPMWRES)) { // Configure the PWM channel
		Serial.printf("LEDc attach sucess");
	}
	
	sbus_tx.Begin();
	data.failsafe = false;
	data.ch17 = true;
	data.ch18 = true;
	data.lost_frame = false;
	for (int i = 500; i > 172; i--) {
		for (int j = 0; j < 16; j++) {
			data.ch[j] = i;
		}
		sbus_tx.data(data);
		sbus_tx.Write();
	}

	// Set device as a Wi-Fi Station
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);
	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_mac(WIFI_IF_STA, mac_addr);
	esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20);
	esp_wifi_set_storage(WIFI_STORAGE_RAM);
	esp_wifi_set_ps(WIFI_PS_NONE);
	esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
	esp_wifi_start();

	if (esp_now_init() != ESP_OK) { // Init ESP-NOW
		Serial.println("Error initializing ESP-NOW");
		return;
	}
	esp_wifi_config_espnow_rate(WIFI_IF_STA, WIFI_PHY_RATE_24M);
	esp_wifi_start();
	esp_now_register_recv_cb(data_recv_cb); // Register the receive callback
	readMacAddress();

	xPosPID.SetMode(AUTOMATIC);
	yPosPID.SetMode(AUTOMATIC);
	zPosPID.SetMode(AUTOMATIC);
	yawPosPID.SetMode(AUTOMATIC);
	xVelPID.SetMode(AUTOMATIC);
	yVelPID.SetMode(AUTOMATIC);
	zVelPID.SetMode(AUTOMATIC);
	
	xPosPID.SetSampleTime(0); // Sample rate is determined by main loop
	yPosPID.SetSampleTime(0);
	zPosPID.SetSampleTime(0);
	yawPosPID.SetSampleTime(0);
	xVelPID.SetSampleTime(0);
	yVelPID.SetSampleTime(0);
	zVelPID.SetSampleTime(0);

	xPosPID.SetOutputLimits(-MAX_VEL, MAX_VEL);
	yPosPID.SetOutputLimits(-MAX_VEL, MAX_VEL);
	zPosPID.SetOutputLimits(-MAX_VEL, MAX_VEL);
	yawPosPID.SetOutputLimits(-1, 1);
	xVelPID.SetOutputLimits(-1, 1);
	yVelPID.SetOutputLimits(-1, 1);
	zVelPID.SetOutputLimits(-1, 1);

	// EEPROM.begin(EEPROM_SIZE);
	// xTrim = EEPROM.read(0);
	// yTrim = EEPROM.read(1);
	// zTrim = EEPROM.read(2);
	// yawTrim = EEPROM.read(3);

	lastPing = micros();
	lastLoopTime = micros();
	lastSbusSend = micros();
}

void loop() {
	while (micros() - lastLoopTime < 1e6 / loopFrequency) { yield(); }
	lastLoopTime = micros();

	if (micros() - lastPing > 2e6) { // safety timmed killswitch 
		armed = false;
	}

	if (armed) {
		data.ch[4] = 1800;
	} else {
		data.ch[4] = 172;
		resetPid(xPosPID, -MAX_VEL, MAX_VEL);
		resetPid(yPosPID, -MAX_VEL, MAX_VEL);
		resetPid(zPosPID, -MAX_VEL, MAX_VEL);
		resetPid(yawPosPID, -1, 1);
		resetPid(xVelPID, -1, 1);
		resetPid(yVelPID, -1, 1);
		resetPid(zVelPID, -1, 1);
	}

	xPosPID.Compute();
	yPosPID.Compute();
	zPosPID.Compute();
	yawPosPID.Compute();

	xVelPID.Compute();
	yVelPID.Compute();
	zVelPID.Compute();

	int xPWM = 992 + (xVelOutput * 811) + xTrim;
	int yPWM = 992 + (yVelOutput * 811) + yTrim;
	int zPWM = 992 + (Z_GAIN * zVelOutput * 811) + zTrim;
	int yawPWM = 992 + (yawPosOutput * 811) + yawTrim;
	// double groundEffectMultiplier = 1 - groundEffectCoef*pow(((2*ROTOR_RADIUS) / (4*(zPos-groundEffectOffset))), 2);
	// zPWM *= max(0., groundEffectMultiplier);
	zPWM = zPWM > 1800 ? 1800 : zPWM; // temporary limit to avoid going too high and disarm
	zPWM = ((armed && millis() - timeArmed > 100) ? zPWM : 220);

	data.ch[0] = -yPWM;
	data.ch[1] = xPWM;
	data.ch[2] = zPWM;
	data.ch[3] = yawPWM;

	// for (int i = 0; i < 5; ++i) {
	//   if (rep_data[i])
	//     data.ch[i] = rep_data[i];
	// }

	if (micros() - lastSbusSend > 1e6 / sbusFrequency) {
		lastSbusSend = micros();
		sbus_tx.data(data);
		sbus_tx.Write();
		// Serial.printf("PWM x: %d, y: %d, z: %d, yaw: %d\n", xPWM, yPWM, zPWM, yawPWM);
	}
}
