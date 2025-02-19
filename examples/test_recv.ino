#include <Arduino.h>
#include "CAN_Teensy4.0.h"

/*test_data*/
uint8_t len;
int8_t recv_data8[8];
int16_t recv_data16[4];

void setup() {
    Serial.begin(115200);

    can_init();

    delay(1000);
}

void loop() {
    can_recv8(0x301, recv_data8, len);
    Serial.print("recv_data8: ");
    for (int i = 0; i < len; i++) {
        Serial.print(recv_data8[i]);
        Serial.print(" ");
    }
    Serial.println();

    can_recv16(0x302, recv_data16, len);
    Serial.print("recv_data16: ");
    for (int i = 0; i < len; i++) {
        Serial.print(recv_data16[i]);
        Serial.print(" ");
    }
    Serial.println();

    delay(1000);
}