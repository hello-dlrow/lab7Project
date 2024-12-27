#ifndef PWM_H
#define PWN_H

#include <Arduino.h>
#include <math.h>

#define abs(x) ((x)>0?(x):-(x))             


#define MAXDUTY1 10.329 // Max input dutycircle from input pwm
#define MINDUTY1 6.133  // Min inpyt dutycircle from input pwm
#define MAXDUTY2 9.7    // Max input dutycircle from input pwm
#define MINDUTY2 5.629  // Min inpyt dutycircle from input pwm
#define SENSITIVITY 0.5
#define DEADZONE 0.05

const int LEFT_PWM_OUTPUT_PIN = 5;
const int RIGHT_PWM_OUTPUT_PIN = 6;

// define outpur PWM to control motor
struct MotorOutput {
  float forward;
  float direction;

  int8_t leftRotate;
  int8_t rightRotate;

  float leftPWM;
  float rightPWM;
};

// throttle from -1 to 1 (get from user controller throttle input)
// steering from -1 to 1 (get from user controller steering input)
// steeringFactor 0~2
// deadzone deafault 10
// pwm output from 0 ~ 1
void calculateDifferentialDrive(MotorOutput *myMotor);
void calculateRotateSpeed(MotorOutput *myMotor, double w, double v);

void startMotor(MotorOutput *myMotor);

// transfer input 50HZ, 1~20ms(depand on measurment) pulse width pwm input to
// -1~1 float number
float pwmNormalize(unsigned long pulse_time, unsigned long last_time,
                   float min_duty, float max_duty);

#endif
