#ifndef MP6050_H
#define MP6050_H
#include <Wire.h>
#include <Arduino.h>
#include <stdio.h>

typedef struct {
    float AccX, AccY, AccZ;        // 加速度计数据
    float GyroX, GyroY, GyroZ;     // 陀螺仪数据

    float accAngleX, accAngleY;    // 加速度计计算角度
    float gyroAngleX, gyroAngleY, gyroAngleZ; // 陀螺仪计算角度

    float roll, pitch, yaw;        // 三个方向角

    float AccErrorX, AccErrorY;    // 加速度计误差
    float GyroErrorX, GyroErrorY, GyroErrorZ; // 陀螺仪误差

    float elapsedTime, currentTime, previousTime; // 时间相关变量

    int MPUADDR = 0x68; // MPU6050 I2C address

    int c ; //error calculation counter
} MPU6050;

void initMP6050(MPU6050 *myMPUData);
void readMP6050(MPU6050 *myMPUData);
void calculate_IMU_error(MPU6050 *myMPUData);

// 定义MPU6050传感器数据结构体


#endif
