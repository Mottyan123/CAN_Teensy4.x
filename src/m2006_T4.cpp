//M3508ように変更
#include <Arduino.h>
#include <m2006_T4.h>

#include <FlexCAN_T4.h>

static uint16_t angle = 0;
static uint16_t rpm = 0;
static uint16_t torque = 0;

static uint8_t top = 0;
static uint8_t bottom = 0;
static int16_t data = 0;

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;

CAN_message_t msg;

// setup()で呼び出す
bool can_init(int speed) {
    can1.begin();
    can1.setBaudRate(speed);
    can2.begin();
    can2.setBaudRate(speed);
    can3.begin();
    can3.setBaudRate(speed);

    return true;  
}

// M2006に送信するデータを作成
void m2006_make_data(int16_t data_in[8], uint8_t data_out1[8], uint8_t data_out2[8]) {
  // 0x200用のデータを作成（ID1-4）
  for (int i = 0; i < 4; i++) {
      data_out1[0 + (i * 2)] = (data_in[i] >> 8) & 0xFF;
      data_out1[1 + (i * 2)] = data_in[i] & 0xFF;
  }
  // 0x1FF用のデータを作成（ID5-8）
  for (int i = 0; i < 4; i++) {
      data_out2[0 + (i * 2)] = (data_in[i + 4] >> 8) & 0xFF;
      data_out2[1 + (i * 2)] = data_in[i + 4] & 0xFF;
  }
}

// M2006にデータを送信
bool m2006_send_data(uint8_t data_out1[8], uint8_t data_out2[8]) {
  bool success0x200 = false;
  bool success0x1FF = false;
  
  //CAN.beginPacket(0x200); // 0x200は，M2006のCAN ID(1-4)
  msg.id = 0x200;

  //CAN.write(data_out1, 8); // 8 bytes
  for(int i = 0; i < 8; i++) {
      msg.buf[i] = data_out1[i];
  }
  can1.write(msg);
  success0x200 = true;


  msg.id = 0x1FF;

  for(int i = 0; i < 8; i++) {
      msg.buf[i] = data_out2[i];
  }
  can1.write(msg);
  success0x1FF = true;

  return success0x200 && success0x1FF;
}

bool m2006_send_data2(uint8_t data_out1[8], uint8_t data_out2[8]) {
  bool success0x200 = false;
  bool success0x1FF = false;
  
  //CAN.beginPacket(0x200); // 0x200は，M2006のCAN ID(1-4)
  msg.id = 0x200;

  //CAN.write(data_out1, 8); // 8 bytes
  for(int i = 0; i < 8; i++) {
      msg.buf[i] = data_out1[i];
  }
  can3.write(msg);
  success0x200 = true;

  return success0x200 && success0x1FF;
}


// M2006からデータを受信
bool m2006_read_data(u_int id, int16_t mangle[8], int16_t mrpm[8], int16_t mtorque[8]) {
  if(can1.read(msg)){        
    angle = 0;
    rpm = 0;
    torque = 0;
    top = 0;
    bottom = 0;
    data = 0;
    
    if(id==msg.id){
      int id = msg.id;
      for (int i = 0; i < 4; i++){
        top = msg.buf[i*2];
        bottom = msg.buf[i*2+1];
        data = (top << 8) | bottom;
        if (i == 0) angle = (float)(data*360/8192.0);
        if (i == 1) rpm = (float)(data);
        if (i == 2) torque = (float)(data);
        if (i == 3) data = 0;
      }
      mangle[id - 0x201] = angle;
      mrpm[id - 0x201] = rpm;
      mtorque[id - 0x201] = torque;
      return true;
    }else{
      return false;
    }
  }
  return false;
}

void m2006_read_data2(u_int id, int16_t mangle[8], int16_t mrpm[8], int16_t mtorque[8]) {
  if(can3.read(msg)){        
    angle = 0;
    rpm = 0;
    torque = 0;
    top = 0;
    bottom = 0;
    data = 0;
    
    if(id==msg.id){
      int id = msg.id;
      for (int i = 0; i < 4; i++){
        top = msg.buf[i*2];
        bottom = msg.buf[i*2+1];
        data = (top << 8) | bottom;
        if (i == 0) angle = (float)(data*360/8192.0);
        if (i == 1) rpm = (float)(data);
        if (i == 2) torque = (float)(data);
        if (i == 3) data = 0;
      }
      mangle[id - 0x201] = angle;
      mrpm[id - 0x201] = rpm;
      mtorque[id - 0x201] = torque;
    }else{
      return;
    }
  }
}

bool can2_readf(u_int id, float can2_data[]){
  uint8_t recieveData[8];
  if(can2.read(msg)){
      if(id==msg.id){
          int len = msg.len;
          for ( uint8_t i = 0; i < len; i++ ) {
              recieveData[i] = msg.buf[i];
          }
          //float型に変換
          //memccpy(can2_data, recieveData, sizeof(float), len);
          memcpy(can2_data, recieveData, sizeof(float));
          return true;
      }else{
        return false;
      }
  }
  return false;
}

bool can2_readi(u_int id, int can2_data[]){
  uint8_t recieveData[8];
  if(can2.read(msg)){
      if(id==msg.id){
          int len = msg.len;
          for ( uint8_t i = 0; i < len; i++ ) {
              recieveData[i] = msg.buf[i];
          }
          //int型に変換
          //memccpy(can2_data, recieveData, sizeof(int), len);
          memcpy(can2_data, recieveData, sizeof(int));
          return true;
      }else{
        return false;
      }
  }
  return false;
}