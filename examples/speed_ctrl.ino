#include <Arduino.h>
#include "pid.h"
#include "m2006_T4.h"
#include <TeensyThreads.h>

//CAN
#define CAN_SPEED 1000000 // 1Mbps

float before_time = 0.0;

int motor_list[8] = {0, 2, 2, 2, 2, 2, 2, 2};//モータの種類(0=m2006,1=m3508,2=なし)
//モード(0=速度制御,1=位置制御)
int16_t mode[8] = {0, 1, 1, 1, 1, 1, 1, 1};

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
float first_angle[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float rpm[8] = {0.0, 0.0, 0.0, 0.0, 0.0,0.0, 0.0, 0.0};//回転数
float torque[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};//トルク

//角度制御用変数
float dif_angle[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float before_angle[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float output_angle[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float count[8] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

//外部入力
int16_t input_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void one(){
  while(1){
    target_rpm[0] = 50.0;

    // target_pos[0] = 0.0;
    // threads.delay(1000);
    // target_pos[0] = 90.0;
    // threads.delay(1000);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("Start");

  while(!can_init(CAN_SPEED)) {
  Serial.println("CAN init fail, retry...");
  delay(1000);
  }

  for(int i = 0; i < 8; i++) {
    //設定したモータのデータを読み込む
    if(motor_list[i]==2){
    }else{
    while(!m2006_read_data(id[i], mangle, mrpm, mtorque)) {
    }
    }
    angle[i] = (float)mangle[i];
    rpm[i] = (float)mrpm[i];
    torque[i] = (float)mtorque[i];
    // before_angle[i] = angle[i];
    // first_angle[i] = angle[i];
  }
  Serial.println(angle[0]);
  
  threads.addThread(one);//スレッドの追加
}

void loop() {
  float dt = millis() - before_time;
  before_time = millis();
  //DJIモータのデータ読み込み
  for(int i = 0; i < 8; i++) {
    if(motor_list[i]==2){
    }else{
      while(!m2006_read_data(id[i], mangle, mrpm, mtorque)) {
      }
      angle[i] = (float)mangle[i];
      rpm[i] = (float)mrpm[i];
      torque[i] = (float)mtorque[i];

      dif_angle[i] = angle[i] - before_angle[i];
      before_angle[i] = angle[i];
      if(dif_angle[i] > 180.0){
        count[i] -= 1.0;
      }else if(dif_angle[i] < -180.0){
        count[i] += 1.0;
      }
      if(motor_list[i]==0){
        output_angle[i] = ((count[i]*360.0 + angle[i]))/36.0;//m2006の出力軸角度
      }else if(motor_list[i]==1){
        output_angle[i] = ((count[i]*360.0 + angle[i]))/19.0;//m3508の出力軸角度
      }
      //output_angle[i] = ((count[i]*360.0 + angle[i]))/36.0;
    }
  }
  for(int i = 0; i < 8; i++) {
    if(motor_list[i]==2){
    }else{
      if(mode[i] == 0) {
        current_data[i] = speed_PI(id[i], motor_list[i], target_rpm[i], rpm[i], torque[i], dt);
      } else if(mode[i] == 1) {
        current_data[i] = position_PPI(id[i], motor_list[i], target_pos[i], output_angle[i], first_angle[i], rpm[i], torque[i], dt);
      }
    }
  }
  //DJIに電流値を送信用のデータに変換する
  m2006_make_data(current_data, send_data1, send_data2);
  //DJIにデータを送信する
  m2006_send_data(send_data1, send_data2);
  //Serial.println(rpm[0]);
  Serial.println(">rpm:"+String(rpm[0]/36));//この書き方で変数をteleplotで表示することができる。
  Serial.println(">angle:"+String(output_angle[0]));//この書き方で変数をteleplotで表示することができる。

  //Serial.println("output_angle[0]:"+String(output_angle[0])+"output_angle[2]:"+String(output_angle[2])+"current_data[0]:"+String(current_data[0])+"current_data[2]:"+String(current_data[2]));
}