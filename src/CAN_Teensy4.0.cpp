#include "CAN_Teensy4.0.h"

/*onedeta_size: -128~127*/
bool can_send8(uint16_t id, int8_t data[], uint8_t size) {
    if (size > 8) {
        return false;
    }

    for (int i = 0; i < size; i++) {
        msg.buf[i] = (uint8_t)data[i];
    }

    msg.id = id;
    msg.len = size;

    return can2.write(msg);
}

/*onedeta_size: -32768~32767*/
bool can_send16(uint16_t id, int16_t data[], uint8_t size) {
    if (size > 4) {
        return false;
    }

    for (int i = 0; i < size; i++) {
        msg.buf[i*2] = (uint8_t)(data[i] & 0xFF);
        msg.buf[i*2+1] = (uint8_t)((data[i] >> 8) & 0xFF);
    }

    msg.id = id;
    msg.len = size*2;

    return can2.write(msg);
}

/*8bit_data 受信*/
void can_recv8(uint16_t id, int8_t data[]) {
    if (can2.read(msg)) {
        if (msg.id == id) {
            uint8_t size = msg.len;

            for (int i = 0; i < size; i++) {
                data[i] = (int8_t)msg.buf[i];
            }
        }
    }
}

/*16bit_data 受信*/
void can_recv16(uint16_t id, int16_t data[]) {
    if (can2.read(msg)) {
        if (msg.id == id) {
            uint8_t size = msg.len;

            for (int i = 0; i < size/2; i++) {
                data[i] = (int16_t)((msg.buf[i*2+1] << 8) | msg.buf[i*2]);
            }
        }
    }
}