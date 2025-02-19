#include <Arduino.h>
#include "CAN_Teensy4.0.h"

/*test_data*/
int8_t send_data8[] = {0, 100, -100, 127, -128, 1, 2, 3};
int16_t send_data16[] = {0, 10000, -10000, 32767};
float send_dataF16[] = {0.0, 180.1234, 360.4567, 654.7890};

void setup() {
    Serial.begin(115200);

    can_init();

    delay(1000);
}

void loop() {
    if (!can_send8(0x301, send_data8, 8)) {
        Serial.println("can_send8 fail");
    }

    if (!can_send16(0x302, send_data16, 4)) {
        Serial.println("can_send16 fail");
    }

    if (!can_sendF16(0x303, send_dataF16, 4)) {
        Serial.println("can_sendU16 fail");
    }

    delay(1000);
}