#include <main.h>

// 存储输入PWM相关变量
unsigned long pulseInTime1;
unsigned long lastTime1;
bool newPulse1 = false;

unsigned long pulseInTime2;
unsigned long lastTime2;
bool newPulse2 = false;



// 设置PID；
PID pid(0.6, 0, 0.4);
double setpoint = 0;

double delta_t = 0;
double cur_start = 0;
double cur_end = 0;

double error = 0;
double output = 0;

//串口通讯
bool stringComplete;
 String str;

//mpu6050
MPU6050 myMPUData;
MotorOutput myMotor = {0};

// 中断处理函数声明
void pwmHandler1();
void pwmHandler2();

void setup() {
  initSerial();
  initpin();

  // pinMode(buzzerPin, OUTPUT); //Buzzer
  //  pinMode(9, OUTPUT);  //LED1 Forward
  //  pinMode(10, OUTPUT);	//LED2 Backward
  //  pinMode(14, OUTPUT);	//LED3 RIGHT
  //  pinMode(17, OUTPUT); 	//LED4 LEFT

  initMP6050(&myMPUData);
  myMPUData.MPUADDR = 0x68; // MPU6050 I2C address
  calculate_IMU_error(&myMPUData);
  // initial state for roll, pitch and yaw angle
  myMPUData.roll = 0;
  myMPUData.pitch = 0;
  myMPUData.yaw = 0;

  // 配置外部中断
  attachInterrupt(digitalPinToInterrupt(pwmPin1), pwmHandler1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pwmPin2), pwmHandler2, CHANGE);
}

void loop() {
  
  while (Serial.available()) {
    char inChar = (char)Serial.read();

    if (inChar == '\n') {
      stringComplete = true;
    } 
    else {
      str += inChar;
    }
  }

  if (stringComplete) {
    str.trim();
    error = str.toDouble();
    Serial.println(error);
    str = "";
    stringComplete = false;
  }

  

  cur_end = micros();

  //delta_t = 0.18 s
  delta_t = (cur_end - cur_start) / 1000;

  output = pid.compute(setpoint, error, delta_t);
  cur_start = micros();
  
  calculateRotateSpeed(&myMotor, output, 50);
  #if 0
  Serial.print("left");
  Serial.println(myMotor.leftRotate);
  Serial.print("right");
  Serial.println(myMotor.rightRotate);
  #endif
  startMotor(&myMotor);
  #if 0
  Serial.print("left");
  Serial.println(myMotor.leftRotate);
  Serial.print("right");
  Serial.println(myMotor.rightRotate);
  #endif

#if 0
	if (newPulse1)
	{
		// 计算输入油门PWM占空比的正则化 (-1～1)
		myMotor.forward = pwmNormalize(pulseInTime1, lastTime1, MINDUTY1, MAXDUTY1);
		newPulse1 = false;
	}
	if (newPulse2)
	{
    	// 计算输入转向PWM占空比的正则化 (-1-1)
		myMotor.direction = pwmNormalize(pulseInTime2, lastTime2, MINDUTY2, MAXDUTY2);
		newPulse2 = false;
	}
#endif

#if 0
	//when move forward
	//play music and turn on led
	if(myMotor.forward > 0.2 && abs(myMotor.direction) < 0.2){
		playMario();
		digitalWrite(9, HIGH);
	}
	
	//when move backward
	else if(myMotor.forward < -0.2 && abs(myMotor.direction) < 0.2){
		play8BitTune();
		digitalWrite(10, HIGH);
	}
	
	//stay quiet when not moving and turn off led
	else if(abs(myMotor.forward) < 0.5 || abs(myMotor.direction) > 0.2){
		digitalWrite(9,LOW);
		digitalWrite(10, LOW);
		noTone(buzzerPin);
	}

	//turn left led3 blink
	if(myMotor.direction > 0.2){
		digitalWrite(14, HIGH);
	}
	//turn right led4 blink
	else if(myMotor.direction < -0.2){
		digitalWrite(17, HIGH);
	}
	else{
		digitalWrite(14, LOW);
		digitalWrite(17, LOW);
	}
#endif

#if 0
	calculateDifferentialDrive(&myMotor);
	startMotor(&myMotor);
#endif

// test motor control signal
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

#if 0
  readMP6050(&myMPUData);

  // apply filter
  myMPUData.roll = predict(myMPUData.roll);
  myMPUData.pitch = predict(myMPUData.pitch);
  myMPUData.yaw = predict(myMPUData.yaw);

  myMPUData.roll =
      update(myMPUData.roll, myMPUData.accAngleX, myMPUData.gyroAngleX, K1, K2);
  myMPUData.pitch = update(myMPUData.pitch, myMPUData.accAngleY,
                           myMPUData.gyroAngleY, K1, K2);
  myMPUData.yaw = update(myMPUData.yaw, 0, myMPUData.gyroAngleZ, 0,
                         0.05); // no accAngle for yaw meansurement
#endif

#if 0
  Serial.print("rotateSpeed:");
  Serial.print(rotateSpeed);
  Serial.print(" Yaw:");
  Serial.print(myMPUData.yaw);
  Serial.print(" Directoin:");
  Serial.print(myMotor.direction);
  Serial.print(" PWMLeft:");
  Serial.print(myMotor.leftPWM);
  Serial.print(" PWMRight:");
  Serial.println(myMotor.rightPWM);
#endif

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
void pwmHandler1() {
  static unsigned long startTime = 0;

  unsigned long currentTime = micros();
  bool currentState = digitalRead(pwmPin1);

  static unsigned int i = 1;

  // period = very first rising edge(time) - very second rising edge(time)
  // after first period, we get pulsewidth in falling edge, get period in next
  // rising egde
  if (currentState && i == 1) {
    startTime = currentTime;
    i++;
  } else if (!currentState) {
    pulseInTime1 = currentTime - startTime;
  } else if (currentState && i == 2) {
    lastTime1 = currentTime - startTime;
    startTime = currentTime;

    newPulse1 = true;
  }
}

void pwmHandler2() {
  static unsigned long startTime = 0;

  unsigned long currentTime = micros();
  bool currentState = digitalRead(pwmPin2);

  static unsigned int i = 1;

  if (currentState && i == 1) {
    startTime = currentTime;
    i++;
  } else if (!currentState) {
    pulseInTime2 = currentTime - startTime;
  } else if (currentState && i == 2) {
    lastTime2 = currentTime - startTime;
    startTime = currentTime;

    newPulse2 = true;
  }
}
