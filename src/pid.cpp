#include <Arduino.h>
#include "pid.h"

//float before_angle = 0.0;
//float count = 0.0;
//float output_angle = 0.0;
float dif_output_angle = 0.0;
int i=0;

//id:モータのCAN-ID,motor_list:モータの種類(0=m2006,1=m3508),target_rpm:目標回転数,rpm:現在の回転数,now_current:現在の電流,current_data:出力電流,dt:時間間隔
int16_t speed_PI(uint id,int motor_list, int motor_power, float target_rpm, float rpm ,float now_current, long dt){
    float Kp_DJI[2] = {50.0, 50.0};//比例ゲイン[0=m2006,1m3508]
    float Ki_DJI[2] = {0.0, 0.0};//積分ゲイン
    float Kt[2] = {0.18, 0.3};//トルク定数(m2006,m3508)
    //電流制限
    int current_limit_H[2] = {10000 * motor_power / 100, 16384 * motor_power / 100};//電流制限(m2006,m3508)
    int current_limit_L[2] = {-10000 * motor_power / 100, -16384 * motor_power / 100};//電流制限(m2006,m3508)
    
    float GEAR_RATIO[2] = {36.0, 19.0};//減速比(m2006,m3508)
    float dif_rpm=0.0;
    float integral_rpm=0.0;
    float prev_rpm=0.0;
    float output_torque=0.0;
    float current_data_f = 0.0;

    int16_t current_data = 0;

    //m2006とm3508で減速比が異なる
    rpm= rpm/GEAR_RATIO[motor_list];
    //Serial.println(rpm);

    dif_rpm = target_rpm - rpm;
    integral_rpm += (dif_rpm + prev_rpm)*dt/ 2.0;
    prev_rpm = dif_rpm;
    output_torque = (Kp_DJI[motor_list] * dif_rpm + Ki_DJI[motor_list] * integral_rpm)/Kt[motor_list];
    //Serial.println(output_torque);
    current_data_f = output_torque - now_current;
    //Serial.println(current_data_f);
    current_data =constrain(current_data_f, current_limit_L[motor_list], current_limit_H[motor_list]);//最大電流を制限
    //Serial.println(current_data);

    return current_data;
}

int16_t position_PPI(uint id, int motor_list, int motor_power, int motor_speed, float target_pos, float angle, float rpm , float now_current, long dt){
    float angle_Kp_DJI[2] = {4.0, 3.0};//角度比例ゲイン[0=m2006,1m3508]
    float Kp_DJI[2] = {40.0, 50.0};//速度比例ゲイン[0=m2006,1m3508]
    float Ki_DJI[2] = {0.0, 0.0};//速度積分ゲイン
    float Kt[2] = {0.18, 0.3};//トルク定数(m2006,m3508)
    //電流制限
    int current_limit_H[2] = {10000 * motor_power / 100, 16384 * motor_power / 100};//電流制限(m2006,m3508)
    int current_limit_L[2] = {-10000 * motor_power / 100, -16384 * motor_power / 100};//電流制限(m2006,m3508)
    
    float GEAR_RATIO[2] = {36.0, 19.0};//減速比(m2006,m3508)

    //位置P制御
    dif_output_angle = target_pos - angle;
    float target_rpm = dif_output_angle * angle_Kp_DJI[motor_list];
    target_rpm = constrain(target_rpm, -400.0 * motor_speed / 100, 400.0 * motor_speed / 100);//最大回転数を制限

    //速度PI制御
    float dif_rpm=0.0;
    float integral_rpm=0.0;
    float prev_rpm=0.0;
    float output_torque=0.0;
    float current_data_f = 0.0;

    int16_t current_data = 0;

    //m2006とm3508で減速比が異なる
    rpm = rpm/GEAR_RATIO[motor_list];
    //Serial.println(rpm);

    dif_rpm = target_rpm - rpm;
    integral_rpm += (dif_rpm + prev_rpm)*dt/ 2.0;
    prev_rpm = dif_rpm;
    output_torque = (Kp_DJI[motor_list] * dif_rpm + Ki_DJI[motor_list] * integral_rpm)/Kt[motor_list];
    //Serial.println(output_torque);
    current_data_f = output_torque - now_current;
    //Serial.println(current_data_f);
    current_data =constrain(current_data_f, current_limit_L[motor_list], current_limit_H[motor_list]);
    //Serial.println(current_data);

    return current_data;
}