#include "pwm.h"

// 通过线速度和角速度计算输出PWM
// 线速度v（-100 ～100）
// 角速度w (-100 ~ 100)
// 假设轮距是1
// leftRotate通过油门控制量，油门实际控制加速度，但是这里假设是速度
// 油门最大值和线速度保持一致
void calculateRotateSpeed(MotorOutput *myMotor, double w, double v) {
  // leftRotate, rightRotate (-100 ~ 100)
  double left = v - w / 2;
  double right = v + w / 2;

  myMotor->leftRotate = left > 100 ? 100 : (left < -100 ? -100 : left);                                
  myMotor->rightRotate = right > 100 ? 100 : (right < -100 ? -100 : right);
  #if 0
  Serial.print("left");
  Serial.println(myMotor->leftRotate);
  Serial.print("right");
  Serial.println(myMotor->rightRotate);
  #endif
}

// 差速转向计算函数
// throttle: 油门值 (-100 到 100)
// steering: 转向值 (-100 到 100)
// steeringFactor: 转向灵敏度 (0.0 到 2.0, 1.0为正常灵敏度)
// deadzone: 死区范围 (-5 到 5)
void calculateDifferentialDrive(MotorOutput *myMotor) {

  // 应用转向灵敏度
  myMotor->direction *= 1;

  // 计算左右电机输出(0 ~ 1)
  myMotor->leftPWM = (myMotor->forward + myMotor->direction + 2) / 4;
  myMotor->rightPWM = (myMotor->forward - myMotor->direction + 2) / 4;

  // 应用死区
  if (abs(myMotor->leftPWM - 0.5) < DEADZONE)
    myMotor->leftPWM = 0.5;
  if (abs(myMotor->rightPWM - 0.5) < DEADZONE)
    myMotor->rightPWM = 0.5;

  // 定义方向
  if (myMotor->leftPWM < 0.5) {
    digitalWrite(15, HIGH);
    digitalWrite(16, LOW);
  } else if (myMotor->leftPWM > 0.5) {
    digitalWrite(15, LOW);
    digitalWrite(16, HIGH);
  }

  if (myMotor->rightPWM > 0.5) {
    digitalWrite(7, LOW);
    digitalWrite(8, HIGH);

  } else if (myMotor->rightPWM < 0.5) {
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
  }

  // 0.5 is deadpoint, calculate the deviation from 0.5, and times 100
  myMotor->rightPWM = abs(myMotor->rightPWM - 0.5) * 100;
  myMotor->leftPWM = abs(myMotor->leftPWM - 0.5) * 100;

#if 0
    Serial.print(" Left Motor: ");
	  Serial.print(myMotor->leftPWM);
    Serial.print(" right Motor: ");
	  Serial.println(myMotor->rightPWM);
#endif
}

// map Rotate Speed to pwm signal
/*
void startMotor(MotorOutput *myMotor) {
  analogWrite(LEFT_PWM_OUTPUT_PIN, map(myMotor->leftPWM, 0, 50, 0, 255));
  analogWrite(RIGHT_PWM_OUTPUT_PIN, map(myMotor->rightPWM, 0, 50, 0, 255));
}
*/
void startMotor(MotorOutput *myMotor) {
  // 定义方向
  if (myMotor->leftRotate < 0) {
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
  }
  if (myMotor->leftRotate > 0) {
    digitalWrite(7, LOW);
    digitalWrite(8, HIGH);
    Serial.println("forward");
  }

  if (myMotor->rightRotate > 0) {
    digitalWrite(15, LOW);
    digitalWrite(16, HIGH);

  } 
  if (myMotor->rightRotate < 0) {
    digitalWrite(15, HIGH);
    digitalWrite(16, LOW);
  }


  #if 0
  Serial.print("left");
  Serial.println(myMotor->leftRotate);
  Serial.print("right");
  Serial.println(myMotor->rightRotate);
  #endif

  analogWrite(LEFT_PWM_OUTPUT_PIN,
              map(abs(myMotor->leftRotate), 0, 100, 0, 255));
  analogWrite(RIGHT_PWM_OUTPUT_PIN,
              map(abs(myMotor->rightRotate), 0, 100, 0, 255));
}
// normalize pwm dutyCycle from -100 to 100(backward and forward, turn right or
// turn left)
float pwmNormalize(unsigned long pulse_time, unsigned long last_time,
                   float min_duty, float max_duty) {

  float dutyCycle = (float)pulse_time / (float)last_time * 100.0;

  float normalized_duty =
      2 * (dutyCycle - min_duty) / (max_duty - min_duty) - 1; // -1 ~ 1

  return normalized_duty;
}
