#include <Arduino.h>
#include "CAN_Teensy4.0.h"

/*test_data*/
int8_t send_data8[] = {0, 100, -100, 127, -128, 1, 2, 3};
int16_t send_data16[] = {0, 10000, -10000, 32767};
uint8_t len;
int8_t recv_data8[8];
int16_t recv_data16[4];

void setup() {
    Serial.begin(115200);

    can_init();

    delay(1000);
}

void loop() {
    if (!can_send8(0x301, send_data8, 8)) {
        Serial.println("can_send8 fail");
    }

    if (!can_send16(0x401, send_data16, 4)) {
        Serial.println("can_send16 fail");
    }

    can_recv8(0x101, recv_data8, len);
    Serial.print("recv_data8: ");
    for (int i = 0; i < len; i++) {
        Serial.print(recv_data8[i]);
        Serial.print(" ");
    }
    Serial.println();

    can_recv16(0x201, recv_data16, len);
    Serial.print("recv_data16: ");
    for (int i = 0; i < len; i++) {
        Serial.print(recv_data16[i]);
        Serial.print(" ");
    }
    Serial.println();

    delay(1000);
}