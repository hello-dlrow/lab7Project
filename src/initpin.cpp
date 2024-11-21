#include "initpin.h"

void initSerial(){
   Serial.begin(115200);
      while (!Serial) {
         ; // 等待串口连接（仅针对 Leonardo）
      }
}

void initpin(){

      // 配置输入引脚
      pinMode(pwmPin1, INPUT);
      pinMode(pwmPin2, INPUT);

      //set up output pins
      pinMode(5, OUTPUT); //for PWM output
      pinMode(6, OUTPUT); //for PWM output

      //direction selector pins
      pinMode(DirPin1, OUTPUT);
      pinMode(DirPin2, OUTPUT);
      pinMode(DirPin3, OUTPUT);
      pinMode(DirPin4, OUTPUT);

}