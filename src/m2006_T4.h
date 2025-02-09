// #ifdef __M2006_T4_H__
// #define __M2006_T4_H__

// #include <Arduino.h>
// #include <FlexCAN_T4.h>

// bool can_init(int speed);
// void m2006_make_data(int16_t data_in[8], uint8_t data_out1[8], uint8_t data_out2[8]);
// bool m2006_send_data(uint8_t data_out1[8], uint8_t data_out2[8]);
// void m2006_read_data(int id, int16_t mangle[8], int16_t mrpm[8], int16_t mtorque[8]);

// #endif

#ifndef __M2006_T4_H__
#define __M2006_T4_H__

#include <Arduino.h>
#include <FlexCAN_T4.h>

bool can_init(int speed = 1000000);
extern FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2; // CANインスタンスのグローバル宣言
extern CAN_message_t msg;
void m2006_make_data(int16_t data_in[8], uint8_t data_out1[8], uint8_t data_out2[8]);
bool m2006_send_data(uint8_t data_out1[8], uint8_t data_out2[8]);
bool m2006_send_data2(uint8_t data_out1[8], uint8_t data_out2[8]);
bool m2006_read_data(u_int id, int16_t mangle[8], int16_t mrpm[8], int16_t mtorque[8]);
void m2006_read_data2(u_int id, int16_t mangle[8], int16_t mrpm[8], int16_t mtorque[8]);
bool can2_readf(u_int id, float can2_data[]);
bool can2_readi(u_int id, int can2_data[]);

#endif