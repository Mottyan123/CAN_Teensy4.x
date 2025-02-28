#include <Arduino.h>
#include <TeensyThreads.h>
#include "CAN_Teensy4.0.h"

//CAN
#define CAN_SPEED 1000000 // 1Mbps

//アブソリュートエンコーダの設定(I2C)
#define SDA_PIN 18
#define SCL_PIN 19
#define DIR_PIN 21 //ピンには何も接続しない

// AS5600_TCA9548A
float offset[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // offsetはエンコーダの原点を変更するためのオフセット値
float current_ABS_angle[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // AS5600の現在の角度(8個分) 

long dt = 0.0;
long before_time = 0.0;

int motor_list[8] = {0, 2, 2, 2, 2, 2, 2, 2}; //モータの種類(0=m2006,1=m3508,2=なし)
int16_t mode[8] = {0, 0, 0, 0, 0, 0, 0, 0}; //モード(0=速度制御,1=位置制御(内部エンコーダ),2=位置制御(外部エンコーダ))
const float gear_ratio = 1.0; //回転軸に対してのエンコーダの回転比 

//DJI_CANID(変更不可)
int id[8] = {0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207, 0x208};

float target_pos[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float target_rpm[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

//DJI送信データ
int16_t current_data[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // M2006の8個分の電流値(16bit)
uint8_t send_data1[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // DJIに送信するデータ(8bit) (ID:1-4)
uint8_t send_data2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // DJIに送信するデータ(8bit) (ID:5-8)

//DJI受信データ
int16_t mangle[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // M2006の8個分の角度値(16bit)
int16_t mrpm[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // M2006の8個分の回転数値(16bit)
int16_t mtorque[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // M2006の8個分のトルク値(16bit)電流値
float angle[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};//角度
float rpm[8] = {0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0};//回転数
float torque[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};//トルク

//角度制御用変数
float output_angle[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float deg[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

void one(){
    while(1){
        target_rpm[0] = 200.0;

        threads.delay(1);
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("Start");

    /*I2Cのピンをプルアップ*/
    pinMode(SDA_PIN, INPUT_PULLUP);
    pinMode(SCL_PIN, INPUT_PULLUP);

    /*tca9548aの初期化*/
    as5600_tca9548a_init(DIR_PIN);
    Serial.println("> AS5600_TCA9548A: Started.");
    delay(1000);

    while(!can_init(CAN_SPEED)) {
        Serial.println("CAN init fail, retry...");
        delay(1000);
    }

    /*DJIモータのデータ読み込み*/
    for(int i = 0; i < 8; i++) {
        if(motor_list[i]==2){
        }
        else{
            while(!m2006_read_data(id[i], mangle, mrpm, mtorque)) {
            }
            angle[i] = (float)mangle[i];
            rpm[i] = (float)mrpm[i];
            torque[i] = (float)mtorque[i];
        }
    }
    threads.addThread(one);//スレッドの追加
}

void loop() {
    dt = millis() - before_time;
    before_time = millis();

    /*DJIモータのデータ読み込み*/
    for(int i = 0; i < 8; i++) {
        if(motor_list[i]==2){
        }
        else{
            while(!m2006_read_data(id[i], mangle, mrpm, mtorque)) {
            }
            angle[i] = (float)mangle[i];
            rpm[i] = (float)mrpm[i];
            torque[i] = (float)mtorque[i];

            /*現在角度を計算＆取得*/
            if(mode[i] == 1) {  
                if(motor_list[i]==0){
                    deg[i] = (rpm[i]/36.0)*360.0/60.0; //rpmから角速度(deg/s)に変換
                    output_angle[i] += deg[i]*dt/1000.0; //degのみに変換
                }
                else if(motor_list[i]==1){
                    deg[i] = (rpm[i]/19.0)*360.0/60.0; //rpmから角速度(deg/s)に変換
                    output_angle[i] += deg[i]*dt/1000.0; //degのみに変換
                }
            }
            else if(mode[i] == 2){
                get_now_angle(i, current_ABS_angle, offset, gear_ratio); //指定したABSエンコーダの角度を取得
            }
        }
    }

    /*各モータのPI制御*/
    for(int i = 0; i < 8; i++) {
        if(motor_list[i]==2){
        }
        else{
            if(mode[i] == 0) {
                current_data[i] = speed_PI(id[i], motor_list[i], target_rpm[i], rpm[i], torque[i], dt);
            } 
            else if(mode[i] == 1) {
                current_data[i] = position_PPI(id[i], motor_list[i], target_pos[i], output_angle[i], rpm[i], torque[i], dt);
            } 
            else if(mode[i] == 2) {
                current_data[i] = position_PPI(id[i], motor_list[i], target_pos[i], current_ABS_angle[i], rpm[i], torque[i], dt);
            }
        }
    }

    /*CANで制御値送信*/
    m2006_make_data(current_data, send_data1, send_data2); //DJIに電流値を送信用のデータに変換する
    m2006_send_data(send_data1, send_data2); //DJIにデータを送信する
}