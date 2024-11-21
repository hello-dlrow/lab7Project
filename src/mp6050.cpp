/*
   Arduino and MPU6050 Accelerometer and Gyroscope Sensor Tutorial
   by Dejan, https://howtomechatronics.com
*/
#include "mp6050.h"


void initMP6050(MPU6050 *myMPUData){
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(myMPUData->MPUADDR);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  /*
  // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
  Wire.beginTransmission(MPU);
  Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);
  // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
  Wire.endTransmission(true);
  delay(20);
  */
  // Call this function if you need to get the IMU error values for your module
  delay(20);
}

void readMP6050(MPU6050 *myMPUData) {

  // === Read acceleromter data === //
  Wire.beginTransmission(myMPUData->MPUADDR);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(myMPUData->MPUADDR, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  myMPUData->AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  myMPUData->AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  myMPUData->AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  myMPUData->accAngleX = (atan(myMPUData->AccY / sqrt(pow(myMPUData->AccX, 2) + pow(myMPUData->AccZ, 2))) * 180 / PI) - myMPUData->AccErrorX; // myMPUData->AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  myMPUData->accAngleY = (atan(-1 * myMPUData->AccX / sqrt(pow(myMPUData->AccY, 2) + pow(myMPUData->AccZ, 2))) * 180 / PI) - myMPUData->AccErrorY; // myMPUData->AccErrorY ~(-1.58)
  

  // === Read gyroscope data === //
  myMPUData->previousTime = myMPUData->currentTime;        // Previous time is stored before the actual time read
  myMPUData->currentTime = millis();            // Current time actual time read
  myMPUData->elapsedTime = (myMPUData->currentTime - myMPUData->previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(myMPUData->MPUADDR);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(myMPUData->MPUADDR, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  myMPUData->GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  myMPUData->GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  myMPUData->GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  myMPUData->GyroX = myMPUData->GyroX - myMPUData->GyroErrorX; // GyroErrorX ~(-0.56)
  myMPUData->GyroY = myMPUData->GyroY - myMPUData->GyroErrorY; // GyroErrorY ~(2)
  myMPUData->GyroZ = myMPUData->GyroZ - myMPUData->GyroErrorZ; // GyroErrorZ ~ (-0.8)
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  myMPUData->gyroAngleX = myMPUData->gyroAngleX + myMPUData->GyroX * myMPUData->elapsedTime; // deg/s * s = deg
  myMPUData->gyroAngleY = myMPUData->gyroAngleY + myMPUData->GyroY * myMPUData->elapsedTime;
  myMPUData->gyroAngleZ = myMPUData->gyroAngleZ + myMPUData->GyroZ * myMPUData->elapsedTime;



  // Print the values on the serial monitor
  #if 0
  Serial.print("gyro roll:");
  Serial.print(myMPUData->gyroAngleX);
  Serial.print(" acc roll:");
  Serial.print(myMPUData->accAngleX);
  Serial.print(" gyro pitch:");
  Serial.print(myMPUData->gyroAngleY);
  Serial.print(" acc pitch:");
  Serial.print(myMPUData->accAngleY);
  Serial.print(" gyro yaw:");
  Serial.println(myMPUData->yaw);
  #endif
  #if 0
  Serial.print("Roll:");
  Serial.print(myMPUData->roll);
  Serial.print("  Pitch:");
  Serial.print(myMPUData->pitch);
  Serial.print("  Yaw:");
  Serial.println(myMPUData->yaw);
  #endif
}

void calculate_IMU_error(MPU6050 *myMPUData) {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (myMPUData->c < 200) {
    Wire.beginTransmission(myMPUData->MPUADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(myMPUData->MPUADDR, 6, true);
    myMPUData->AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    myMPUData->AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    myMPUData->AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    myMPUData->AccErrorX = myMPUData->AccErrorX + ((atan((myMPUData->AccY) / sqrt(pow((myMPUData->AccX), 2) + pow((myMPUData->AccZ), 2))) * 180 / PI));
    myMPUData->AccErrorY = myMPUData->AccErrorY + ((atan(-1 * (myMPUData->AccX) / sqrt(pow((myMPUData->AccY), 2) + pow((myMPUData->AccZ), 2))) * 180 / PI));
    myMPUData->c++;
  }
  //Divide the sum by 200 to get the error value
  myMPUData->AccErrorX = myMPUData->AccErrorX / 200;
  myMPUData->AccErrorY = myMPUData->AccErrorY / 200;
  myMPUData->c = 0;
  // Read gyro values 200 times
  while (myMPUData->c < 200) {
    Wire.beginTransmission(myMPUData->MPUADDR);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(myMPUData->MPUADDR, 6, true);
    myMPUData->GyroX = Wire.read() << 8 | Wire.read();
    myMPUData->GyroY = Wire.read() << 8 | Wire.read();
    myMPUData->GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    myMPUData->GyroErrorX = myMPUData->GyroErrorX + (myMPUData->GyroX / 131.0);
    myMPUData->GyroErrorY = myMPUData->GyroErrorY + (myMPUData->GyroY / 131.0);
    myMPUData->GyroErrorZ = myMPUData->GyroErrorZ + (myMPUData->GyroZ / 131.0);
    myMPUData->c++;
  }
  myMPUData->c = 0;
  //Divide the sum by 200 to get the error value
  myMPUData->GyroErrorX = myMPUData->GyroErrorX / 200;
  myMPUData->GyroErrorY = myMPUData->GyroErrorY / 200;
  myMPUData->GyroErrorZ = myMPUData->GyroErrorZ / 200;
  // Print the error values on the Serial Monitor
  #if 0
  Serial.print("AccErrorX: ");
  Serial.print(myMPUData->AccErrorX);
  Serial.print("  AccErrorY: ");
  Serial.print(myMPUData->AccErrorY);
  Serial.print("  GyroErrorX: ");
  Serial.print(myMPUData->GyroErrorX);
  Serial.print("  GyroErrorY: ");
  Serial.print(myMPUData->GyroErrorY);
  Serial.print("  GyroErrorZ: ");
  Serial.println(myMPUData->GyroErrorZ);
  #endif
}