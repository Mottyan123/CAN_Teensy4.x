#include <Arduino.h>
#include "pid.h"

//float before_angle = 0.0;
//float count = 0.0;
//float output_angle = 0.0;
float dif_output_angle = 0.0;
int i=0;

//id:CAN-ID,motor_list:(0=m2006,1=m3508),target_rpm,rpm,now_current,current_data,dt
int16_t speed_PI(uint id,int motor_list, float target_rpm, float rpm ,float now_current, float dt){
    float Kp_DJI[2] = {50.0, 50.0};//[0=m2006,1m3508]
    float Ki_DJI[2] = {0.0, 0.0};//[0=m2006,1m3508]
    float Kt[2] = {0.18, 0.3};//(m2006,m3508)
    
    int current_limit_H[2] = {10000, 16384};//(m2006,m3508)
    int current_limit_L[2] = {-10000, -16384};//(m2006,m3508)
    
    float GEAR_RATIO[2] = {36.0, 19.0};//(m2006,m3508)
    float dif_rpm=0.0;
    float integral_rpm=0.0;
    float prev_rpm=0.0;
    float output_torque=0.0;
    float current_data_f = 0.0;

    int16_t current_data = 0;

    //m2006
    rpm= rpm/GEAR_RATIO[motor_list];
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

int16_t position_PPI(uint id, int motor_list, float target_pos, float angle, float first_angle, float rpm , float now_current, float dt){
    float angle_Kp_DJI[2] = {5.0, 3.0};//[0=m2006,1m3508]
    float Kp_DJI[2] = {50.0, 50.0};//[0=m2006,1m3508]
    float Ki_DJI[2] = {0.0, 0.0};//[0=m2006,1m3508]
    float Kt[2] = {0.18, 0.3};//(m2006,m3508)
    
    int current_limit_H[2] = {10000, 16384};//(m2006,m3508)
    int current_limit_L[2] = {-10000, -16384};//(m2006,m3508)
    
    float GEAR_RATIO[2] = {36.0, 19.0};//(m2006,m3508)

    // float dif_angle = angle - before_angle;
    // before_angle = angle;
    // if(dif_angle > 180.0){
    //     count -= 1.0;
    // }else if(dif_angle < -180.0){
    //     count += 1.0;
    // }
    // 
    // output_angle = ((count*360.0+angle) - first_angle)/GEAR_RATIO[motor_list];
    // // Serial.print("first_angle: ");
    // // Serial.print(first_angle);
    // // Serial.print("output_angle: ");
    // // Serial.println(output_angle);

    
    dif_output_angle = target_pos - angle;
    float target_rpm = dif_output_angle * angle_Kp_DJI[motor_list];
    target_rpm = constrain(target_rpm, -400.0, 400.0);

    
    float dif_rpm=0.0;
    float integral_rpm=0.0;
    float prev_rpm=0.0;
    float output_torque=0.0;
    float current_data_f = 0.0;

    int16_t current_data = 0;

    //m2006
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
