#ifndef __PID_H__
#define __PID_H__

#include <Arduino.h>

int16_t speed_PI(uint id,int motor_list, int motor_power, float target_rpm, float rpm, float now_current, long dt);
int16_t position_PPI(uint id, int motor_list, int motor_power, int motor_speed, float target_pos, float angle, float rpm, float now_current, long dt);

#endif