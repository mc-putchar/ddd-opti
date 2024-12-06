#pragma once

#include <cstdint>

#define S_TEL_BAT 1
typedef struct __attribute__((packed)) s_tel_bat
{
  uint8_t id; // 1 S_TEL_BAT
  uint8_t index;
  uint16_t volt;
  int16_t cur;
}t_tel_bat;

#define S_TEL_ATITU 2
typedef struct __attribute__((packed)) s_tel_atitu
{
  uint8_t id;  // 2 S_TEL_ATITU
  uint8_t index;
  float pitch;
  float roll;
  float yaw;
}t_tel_atitu;

#define S_TEL_RC 3
typedef struct __attribute__((packed)) s_tel_rc
{
  uint8_t id;  // 3 S_TEL_RC
  uint8_t index;
  uint16_t ch_1;
  uint16_t ch_2;
  uint16_t ch_3;
  uint16_t ch_4;
  uint16_t ch_5;
}t_tel_rc;

#define S_POSITION 4
typedef struct __attribute__((packed)) s_position
{
  uint8_t id;  // 4 S_POSITION
  uint8_t index;
  float x;
  float y;
  float z;
  float rot;
}t_position;

#define S_SETPOINT 5
typedef struct __attribute__((packed)) s_setpoint
{
  uint8_t id;  // 5 S_SETPOINT
  uint8_t index;
  float x;
  float y;
  float z;
  float rot;
}t_setpoint;

#define S_TRIM 6
typedef struct __attribute__((packed)) s_trim
{
  uint8_t id;  // 6 S_TRIM
  uint8_t index;
  int16_t x;
  int16_t y;
  int16_t z;
  int16_t yaw;
}t_trim;

#define S_PATH_INFO 7
typedef struct __attribute__((packed)) s_path_info
{
  uint8_t id;  // 7 S_PATH_INFO
  uint8_t index;
  uint16_t len;
}t_path_info;

#define S_LIGHT 8
typedef struct __attribute__((packed)) s_light_info
{
  uint8_t id;  // 8 S_LIGHT
  uint8_t index;
  uint8_t angle;
  uint8_t power;
  uint8_t strobe;
}t_light_info;

// #define S_FRAME_INFO 9
// typedef struct __attribute__((packed)) s_frame_info
// {
//   uint8_t id;  // 9 S_FRAME_INFO
//   uint8_t index;
//   uint16_t frame_nb;
// }t_frame_info;

#define S_FRAME_DATA 10
typedef struct __attribute__((packed)) s_frame_data
{
  uint8_t id;  // 9 S_FRAME_DATA
  uint8_t index;
  float x;
  float y;
  float z;
  float rot;
  uint8_t angle;
  uint8_t power;
  uint8_t strobe;
  uint16_t frame_nb;
}t_frame_data;

#define S_GRAPH 9
typedef struct __attribute__((packed)) s_graph
{
  uint8_t id;                // Unique 9
  uint8_t index;             // Index or sequence number
  int16_t xPWM;              // PWM values
  int16_t yPWM;
  int16_t zPWM;
  int16_t yawPWM;
  float xPID1;          // Velocity outputs
  float yPID1;
  float zPID1;
  float xPID2;          // Position outputs
  float yPID2;
  float zPID2;
  float yawPID2;
  float xPos;                // Current positions
  float yPos;
  float zPos;
  float xPosSetpoint;        // Position setpoints
  float yPosSetpoint;
  float zPosSetpoint;
} t_graph;

