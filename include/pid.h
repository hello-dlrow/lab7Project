#ifndef PID_H
#define PID_H

class PID {
private:
    double kp;      // 比例系数
    double ki;      // 积分系数
    double kd;      // 微分系数

    double prevError;  // 上一次的误差
    double integral;   // 积分值

public:
    // 构造函数
    PID(double kp, double ki, double kd);

    // 设置参数
    void setTunings(double kp, double ki, double kd);

    // 计算PID输出
    double compute(double setpoint, double measuredValue, double deltaTime);

    // 重置积分和误差
    void reset();
};

#endif // PID_H
