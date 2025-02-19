#include "CAN_Teensy4.0.h"

const int SCALE = 100;

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

/*onedeta_size: 0~654.xxx*/
bool can_sendF16(uint16_t id, float data[], uint8_t size) {
    uint16_t re_data[size];

    if (size > 4) {
        return false;
    }

    for (int i = 0; i < size; i++){
        re_data[i] = (uint16_t)(data[i] * SCALE);
    }

    for (int j = 0; j < size; j++) {
        msg.buf[j*2] = (uint8_t)(re_data[j] & 0xFF);
        msg.buf[j*2+1] = (uint8_t)((re_data[j] >> 8) & 0xFF);
    }

    msg.id = id;
    msg.len = size*2;

    return can2.write(msg);
}

/*8bit_data 受信*/
void can_recv8(uint16_t id, int8_t data[], uint8_t &len) {
    if (can2.read(msg)) {
        if (msg.id == id) {
            uint8_t size = msg.len;
            len = size;

            for (int i = 0; i < size; i++) {
                data[i] = (int8_t)msg.buf[i];
            }
        }
    }
}

/*16bit_data 受信*/
void can_recv16(uint16_t id, int16_t data[], uint8_t &len) {
    if (can2.read(msg)) {
        if (msg.id == id) {
            uint8_t size = msg.len;
            len = size/2;

            for (int i = 0; i < size/2; i++) {
                data[i] = (int16_t)((msg.buf[i*2+1] << 8) | msg.buf[i*2]);
            }
        }
    }
}

void can_recvF16(uint16_t id, float data[], uint8_t &len) {
    if (can2.read(msg)) {
        if (msg.id == id) {
            uint8_t size = msg.len;
            len = size/2;
            uint16_t re_data[len];

            for (int i = 0; i < size/2; i++) {            
                re_data[i] = (uint16_t)((msg.buf[i*2+1] << 8) | msg.buf[i*2]);
            }

            for (int j = 0; j < size/2; j++) {
                data[j] = (float)re_data[j] / SCALE;
            }
        }
    }
}