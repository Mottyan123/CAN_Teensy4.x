#include <Arduino.h>
#include "CAN_Teensy4.0.h"

#define __DEBUG__

/*test_data*/
uint8_t len8;
uint8_t len16;
uint8_t lenF16;
int8_t recv_data8[8];
int16_t recv_data16[4];
float recv_dataF16[4];

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial.begin(115200);

    can_init();

    delay(1000);
}

void loop() {
    can_recv8(0x301, recv_data8, len8); //8bit受信
    can_recv16(0x302, recv_data16, len16); //16bit受信
    can_recvF16(0x303, recv_dataF16, lenF16); //float16bit受信

    #ifdef __DEBUG__
    Serial.println();
    Serial.print("recv_data8: ");
    for (int i = 0; i < len8; i++) {
        Serial.print(recv_data8[i]);
        Serial.print(" ");
    }
    Serial.println();

    Serial.print("recv_data16: ");
    for (int i = 0; i < len16; i++) {
        Serial.print(recv_data16[i]);
        Serial.print(" ");
    }
    Serial.println();

    Serial.print("recv_dataF16: ");
    for (int i = 0; i < lenF16; i++) {
        Serial.print(recv_dataF16[i], 4);
        Serial.print(" ");
    }
    Serial.println();
    #endif

    delay(100);
}