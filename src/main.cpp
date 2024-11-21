#include "main.h"

// 存储输入PWM相关变量
unsigned long pulseInTime1;
unsigned long lastTime1;
bool newPulse1 = false;

unsigned long pulseInTime2;
unsigned long lastTime2;
bool newPulse2 = false;

MPU6050 myMPUData;
MotorOutput myMotor = {0};

// 中断处理函数声明
void pwmHandler1();
void pwmHandler2();

void setup()
{
	initSerial();
	initpin();
	pinMode(buzzerPin, OUTPUT);

	initMP6050(&myMPUData);
	myMPUData.MPUADDR = 0x68; // MPU6050 I2C address
	calculate_IMU_error(&myMPUData);
	//initial state for roll, pitch and yaw angle
	myMPUData.roll = 0;
	myMPUData.pitch = 0;
	myMPUData.yaw = 0;

	// 配置外部中断
	attachInterrupt(digitalPinToInterrupt(pwmPin1), pwmHandler1, CHANGE);
	attachInterrupt(digitalPinToInterrupt(pwmPin2), pwmHandler2, CHANGE);
}

void loop()
{
	playMario();
	if (newPulse1)
	{
		// 计算输入油门PWM占空比的正则化 (-1-1)
		myMotor.forward = pwmNormalize(pulseInTime1, lastTime1, MINDUTY1, MAXDUTY1);
		newPulse1 = false;
	}
	if (newPulse2)
	{
    	// 计算输入转向PWM占空比的正则化 (-1-1)
		myMotor.direction = pwmNormalize(pulseInTime2, lastTime2, MINDUTY2, MAXDUTY2);
		newPulse2 = false;
	}
	
	calculateDifferentialDrive(&myMotor);
	startMotor(&myMotor);
	
	#if 1
	Serial.print(" Left Motor: ");
	Serial.print(myMotor.leftPWM);
	Serial.print(" Right Motor: ");
	Serial.println(myMotor.rightPWM);
	#endif

	

//test motor control signal
#if 0
	Serial.print(" Left Motor: ");
	Serial.print(myMotor.leftPWM);
	Serial.print(" Right Motor: ");
	Serial.println(myMotor.rightPWM);
#endif
#if 0
	Serial.print(" Forward: ");
	Serial.print(myMotor.forward);
	Serial.print(" Steering: ");
	Serial.println(myMotor.direction);
#endif

readMP6050(&myMPUData);

//apply filter
myMPUData.roll = predict(myMPUData.roll);
myMPUData.pitch = predict(myMPUData.pitch);
myMPUData.yaw = predict(myMPUData.yaw);

myMPUData.roll = update(myMPUData.roll, myMPUData.accAngleX, myMPUData.gyroAngleX, K1, K2);
myMPUData.pitch = update(myMPUData.pitch, myMPUData.accAngleY, myMPUData.gyroAngleY, K1, K2);
myMPUData.yaw = update(myMPUData.yaw, 0, myMPUData.gyroAngleZ, 0, 0.05); //no accAngle for yaw meansurement

#if 0
Serial.print("Roll: ");
Serial.print(myMPUData.roll);
Serial.print("  Pitch:");
Serial.print(myMPUData.pitch);
Serial.print("  Yaw:");
Serial.println(myMPUData.yaw);
#endif

}

// 中断处理函数实现
void pwmHandler1()
{
	static unsigned long startTime = 0;

	unsigned long currentTime = micros();
	bool currentState = digitalRead(pwmPin1);

	static unsigned int i = 1;

	// period = very first rising edge(time) - very second rising edge(time)
	// after first period, we get pulsewidth in falling edge, get period in next rising egde
	if (currentState && i == 1)
	{
		startTime = currentTime;
		i++;
	}
	else if (!currentState)
	{
		pulseInTime1 = currentTime - startTime;
	}
	else if (currentState && i == 2)
	{
		lastTime1 = currentTime - startTime;
		startTime = currentTime;

		newPulse1 = true;
	}
}

void pwmHandler2()
{
	static unsigned long startTime = 0;

	unsigned long currentTime = micros();
	bool currentState = digitalRead(pwmPin2);

	static unsigned int i = 1;

	if (currentState && i == 1)
	{
		startTime = currentTime;
		i++;
	}
	else if (!currentState)
	{
		pulseInTime2 = currentTime - startTime;
	}
	else if (currentState && i == 2)
	{
		lastTime2 = currentTime - startTime;
		startTime = currentTime;

		newPulse2 = true;
	}
}