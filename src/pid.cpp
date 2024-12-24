#include "PID.h"

// 构造函数
PID::PID(double kp, double ki, double kd)
    : kp(kp), ki(ki), kd(kd), prevError(0), integral(0) {}

// 设置参数
void PID::setTunings(double kp, double ki, double kd) {
    this->kp = kp;
    this->ki = ki;
    this->kd = kd;
}

// 计算PID输出
double PID::compute(double setpoint, double measuredValue, double deltaTime) {
    double error = setpoint - measuredValue;       // 当前误差
    integral += error * deltaTime;                // 积分部分
    double derivative = (error - prevError) / deltaTime; // 微分部分
    prevError = error;                            // 更新上一误差

    // 返回PID控制输出
    return kp * error + ki * integral + kd * derivative;
}

// 重置积分和误差
void PID::reset() {
    integral = 0;
    prevError = 0;
}
