#include <Arduino.h>
#include "CAN_Teensy4.0.h"

#define __DEBUG__

/*test_data*/
int8_t send_data8[] = {0, 100, -100, 127, -128, 1, 2, 3};
int16_t send_data16[] = {0, 10000, -10000, 32767};
float send_dataF16[] = {0.0, 180.1234, 360.4567, 654.7890};
uint8_t len8 = sizeof(send_data8)/sizeof(send_data8[0]);
uint8_t len16 = sizeof(send_data16)/sizeof(send_data16[0]);
uint8_t lenF16 = sizeof(send_dataF16)/sizeof(send_dataF16[0]);

void setup() {
    Serial.begin(115200);

    can_init();

    delay(1000);
}

void loop() {
    if (!can_send8(0x301, send_data8, 8)) { //8bit送信
        Serial.println("can_send8 fail");
    }

    if (!can_send16(0x302, send_data16, 4)) { //16bit送信
        Serial.println("can_send16 fail");
    }

    if (!can_sendF16(0x303, send_dataF16, 4)) { //float16bit送信
        Serial.println("can_sendU16 fail");
    }

    #ifdef __DEBUG__
    Serial.print("send_data8: ");
    for (int i = 0; i < len8; i++) {
        Serial.print(send_data8[i]);
        Serial.print(" ");
    }
    Serial.println();

    Serial.print("send_data16: ");
    for (int i = 0; i < len16; i++) {
        Serial.print(send_data16[i]);
        Serial.print(" ");
    }
    Serial.println();

    Serial.print("send_dataU16: ");
    for (int i = 0; i < lenF16; i++) {
        Serial.print(send_dataF16[i]);
        Serial.print(" ");
    }
    Serial.println();
    #endif

    delay(1000);
}