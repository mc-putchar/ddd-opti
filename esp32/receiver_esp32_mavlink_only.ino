#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include <PID_v1.h>
#include <stdint.h>
#include <MAVLink.h>
#include "sbus.h"
// #include <EEPROM.h>
// UART2 pins for telemetry data
#define RX2_PIN 33


HardwareSerial TelemetrySerial(2);


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



void setup() {
	Serial.begin(115200); // Initialize Serial Monitor
  Serial.println("starting up");

	TelemetrySerial.begin(115200, SERIAL_8N1, RX2_PIN);
	
}

void printMessage(uint8_t *message, uint8_t length) {
    Serial.print("Received MAVLink Message: ");
    for (uint8_t i = 0; i < length; i++) {
        Serial.printf("%02X ", message[i]);
    }
    Serial.println();
}

// Function to process MAVLink messages received via UART2
void readTelemetry() {
    static uint8_t buf[MAVLINK_MAX_PACKET_LEN]; // Buffer for the received message
    static mavlink_message_t msg;
    static mavlink_status_t status;

    while (TelemetrySerial.available() > 0) {
        // Read byte by byte
        uint8_t c = TelemetrySerial.read();
        // Parse the MAVLink message
        if (mavlink_parse_char(MAVLINK_COMM_0, c, &msg, &status)) {
            // Print the entire MAVLink message in raw byte form
            uint8_t message_length = mavlink_msg_to_send_buffer(buf, &msg); // Use this to determine the length of the message
            // printMessage((uint8_t*)&msg, message_length); // Print the parsed message

            // Process the message as before
            switch (msg.msgid) {
                // case MAVLINK_MSG_ID_HEARTBEAT: {
                //     mavlink_heartbeat_t heartbeat;
                //     mavlink_msg_heartbeat_decode(&msg, &heartbeat);
                //     Serial.println("Heartbeat received!");
                //     break;
                // }
                case MAVLINK_MSG_ID_SYS_STATUS: {
                    mavlink_sys_status_t sys_status;
                    mavlink_msg_sys_status_decode(&msg, &sys_status);
                    Serial.printf("Battery Voltage: %.2fV\n", sys_status.voltage_battery * 0.001); // Example conversion
                    Serial.printf("Battery remaining: %.2fV\n", sys_status.battery_remaining);
                    Serial.printf("Battery Current: %.2fA\n", sys_status.current_battery * 0.001); // Example conversion
                    break;
                }
                case MAVLINK_MSG_ID_ATTITUDE: {
                    mavlink_attitude_t attitude;
                    mavlink_msg_attitude_decode(&msg, &attitude);
                    
                    // Print roll, pitch, and yaw in radians
                    Serial.printf("Pitch: %.2f rad\n", attitude.pitch);
                    Serial.printf("Roll: %.2f rad\n", attitude.roll);
                    Serial.printf("Yaw: %.2f rad\n", attitude.yaw);

                    break;
                }
                case MAVLINK_MSG_ID_RC_CHANNELS: { // Assuming aux1 might be part of RC channels
                    mavlink_rc_channels_t rc_channels;
                    mavlink_msg_rc_channels_decode(&msg, &rc_channels);
                    
                    // Assuming AUX1 corresponds to a specific channel (e.g., channel 5)
                    Serial.printf("Aux1 RC: %d\n", rc_channels.chan5_raw); // Adjust the channel as needed
                    
                    break;
                }
                // Add additional cases for other message types you are interested in
            }
        }
    }
}

void loop() {

	readTelemetry();

}
