#ifndef CAN_TEENSY4_H
#define CAN_TEENSY4_H

#include <Arduino.h>
#include "FlexCAN_T4.h"
#include "m2006_T4.h"
#include "pid.h"

bool can_send8(uint16_t id /*CAN ID*/, int8_t data[] /*-128~127*/, uint8_t size);
bool can_send16(uint16_t id /*CAN ID*/, int16_t data[] /*-32768~32767*/, uint8_t size);

void can_recv8(uint16_t id /*CAN ID*/, int8_t data[] /*-128~127*/, uint8_t &len);
void can_recv16(uint16_t id /*CAN ID*/, int16_t data[] /*-32768~32767*/, uint8_t &len);

#endif