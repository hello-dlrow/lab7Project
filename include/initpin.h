// 定义PWM输入引脚
#ifndef INITPIN_H
#define INITPIN_H
#include <Arduino.h>

const int pwmPin1 = 2;  // D2 引脚用于PWM输入
const int pwmPin2 = 3;

const int DirPin1 = 7;
const int DirPin2 = 8;
const int DirPin3 = 15;
const int DirPin4 = 16;


void initpin();
void initSerial();

#endif