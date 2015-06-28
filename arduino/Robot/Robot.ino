#include <Wire.h>           // for I2C
#include <Ultrasonic.h>     // for ultrasonic ranger
#include "ITG3200.h"        // for gyroscope

#define motorSpeedSet             0x82
#define directionSet              0xaa
#define nothing                   0x01    // place holder
#define I2CMotorDriverFB          0x0f    // the address of front-back I2CMotorDriver
#define I2CMotorDriverLR          0x0a    // the address of left-right I2CMotorDriver

// initialize gyroscope
ITG3200 gyro;
float ax,ay,az;

// ultrasonic rangers, numbers specifying digital pins
Ultrasonic ultrasonicLeft(3);
Ultrasonic ultrasonicRight(2);
Ultrasonic ultrasonicFront(5);
Ultrasonic ultrasonicBack(6);
Ultrasonic ultrasonicFrontLeft(8);
Ultrasonic ultrasonicFrontRight(9);
Ultrasonic ultrasonicBackLeft(10);
Ultrasonic ultrasonicBackRight(11);

int directions[4] = {0, 0, 0, 0};
int reactionSpeed = 50;
int moveDist = 100;
int goDist = 400;
int friction = 15;

//////////////////////////////////////////////////////////////////////
//Function to set the 2 DC motor speed
// speed should set to 0~100

void motorSpeedSetFB(unsigned char motorSpeedBack, unsigned char motorSpeedFront)  {
  motorSpeedBack = map(motorSpeedBack, 0, 100, 0, 255);
  motorSpeedFront = map(motorSpeedFront, 0, 100, 0, 255);
  Wire.beginTransmission(I2CMotorDriverFB);  // transmit to device I2CMotorDriverAdd
  Wire.write(motorSpeedSet);                    // set pwm header 
  Wire.write(motorSpeedBack);                      // send pwma 
  Wire.write(motorSpeedFront);                      // send pwmb    
  Wire.endTransmission();                       // stop transmitting
}

void motorSpeedSetLR(unsigned char motorSpeedLeft, unsigned char motorSpeedRight)  {
  motorSpeedLeft = map(motorSpeedLeft, 0, 100, 0, 255);
  motorSpeedRight = map(motorSpeedRight, 0, 100, 0, 255);
  Wire.beginTransmission(I2CMotorDriverLR);  // transmit to device I2CMotorDriverAdd
  Wire.write(motorSpeedSet);                    // set pwm header 
  Wire.write(motorSpeedLeft);                      // send pwma 
  Wire.write(motorSpeedRight);                      // send pwmb    
  Wire.endTransmission();                       // stop transmitting
}

// set the direction of DC motor.
// first two is front, second two is back
void motorDirectionSetFB(unsigned char directionFB)  {      //  Adjust the direction of the motors 0b0000 I4 I3 I2 I1
  Wire.beginTransmission(I2CMotorDriverFB);                 // transmit to device I2CMotorDriverAdd
  Wire.write(directionSet);                                 // Direction control header
  Wire.write(directionFB);                                  // send direction control information
  Wire.write(nothing);                                      // need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();                                   // stop transmitting 
}

// set the direction of DC motor. 
// first two is right, second two is left
void motorDirectionSetLR(unsigned char directionLR)  {      //  Adjust the direction of the motors 0b0000 I4 I3 I2 I1
  Wire.beginTransmission(I2CMotorDriverLR);                 // transmit to device I2CMotorDriverAdd
  Wire.write(directionSet);                                 // Direction control header
  Wire.write(directionLR);                                  // send direction control information
  Wire.write(nothing);                                      // need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();                                   // stop transmitting 
}

// set four motors speed and direction
void motorSpeedDirectionSetFBLR(char frontMotorSpeed, char backMotorSpeed, char leftMotorSpeed, char rightMotorSpeed) {
  // for each wheel, 10 is clockwise and 01 is counter-clockwise
  unsigned char frontBackDirection = 0b0000;
  unsigned char rightLeftDirection = 0b0000;
  if (frontMotorSpeed > 0) {
    frontBackDirection += 0b1000;
  } else if (frontMotorSpeed < 0) {
    frontMotorSpeed = -frontMotorSpeed;
    frontBackDirection += 0b0100;
  }
  if (backMotorSpeed > 0) {
    frontBackDirection += 0b0010;
  } else if (backMotorSpeed < 0) {
    backMotorSpeed = -backMotorSpeed;
    frontBackDirection += 0b0001;
  }
  if (rightMotorSpeed > 0) {
    rightLeftDirection += 0b1000;
  } else if (rightMotorSpeed < 0) {
    rightMotorSpeed = -rightMotorSpeed;
    rightLeftDirection += 0b0100;
  }
  if (leftMotorSpeed > 0) {
    rightLeftDirection += 0b0010;
  } else if (leftMotorSpeed < 0) {
    leftMotorSpeed = -leftMotorSpeed;
    rightLeftDirection += 0b0001;
  }
  motorSpeedSetLR(leftMotorSpeed, rightMotorSpeed);
  motorSpeedSetFB(backMotorSpeed, frontMotorSpeed);
  delay(10);
  motorDirectionSetFB(frontBackDirection);
  motorDirectionSetLR(rightLeftDirection);
}

void setup()  {
  Wire.begin();                 // join i2c bus (address optional for master)
  gyro.init();
  gyro.zeroCalibrate(200, 10);  //sample 200 times to calibrate and it will take 200*10ms
  delayMicroseconds(10000);     //wait for motor driver to initialization
  Serial.begin(9600);
  motorSpeedSetFB(100, 100);
  motorSpeedSetLR(100, 100);
  delay(100);                   //this delay needed
}

void loop()  {

  gyro.getAngularVelocity(&ax,&ay,&az);
  Serial.println(az);

  // if (az > 1) {
  //   directions[0] += reactionSpeed * (az - 1) / 200 / 2;
  //   directions[1] += reactionSpeed * (az - 1) / 200 / 2;
  //   directions[2] += reactionSpeed * (az - 1) / 200 / 2;
  //   directions[3] += reactionSpeed * (az - 1) / 200 / 2;
  // } else if (az < -2) {
  //   directions[0] -= reactionSpeed * (-2 - az) / 200 / 2;
  //   directions[1] -= reactionSpeed * (-2 - az) / 200 / 2;
  //   directions[2] -= reactionSpeed * (-2 - az) / 200 / 2;
  //   directions[3] -= reactionSpeed * (-2 - az) / 200 / 2;
  // }

  ultrasonicFront.MeasureInCentimeters();
  ultrasonicBack.MeasureInCentimeters();
  ultrasonicLeft.MeasureInCentimeters();
  ultrasonicRight.MeasureInCentimeters();
  ultrasonicFrontLeft.MeasureInCentimeters();
  ultrasonicFrontRight.MeasureInCentimeters();
  ultrasonicBackLeft.MeasureInCentimeters();
  ultrasonicBackRight.MeasureInCentimeters();
  int frontDist = ultrasonicFront.RangeInCentimeters;
  int backDist = ultrasonicBack.RangeInCentimeters;
  int leftDist = ultrasonicLeft.RangeInCentimeters;
  int rightDist = ultrasonicRight.RangeInCentimeters;
  int frontLeftDist = ultrasonicFrontLeft.RangeInCentimeters;
  int frontRightDist = ultrasonicFrontRight.RangeInCentimeters;
  int backLeftDist = ultrasonicBackLeft.RangeInCentimeters;
  int backRightDist = ultrasonicBackRight.RangeInCentimeters;
  
  int largestDist = 0;
  char largestDirection = 0;

  // if (frontDist > largestDist) {
  //   largestDist = frontDist;
  //   largestDirection = 0;
  // } else if (frontRightDist > largestDist) {
  //   largestDist = frontRightDist;
  //   largestDirection = 1;
  // } else if (rightDist > largestDist) {
  //   largestDist = rightDist;
  //   largestDirection = 2;
  // } else if (backRightDist > largestDist) {
  //   largestDist = backRightDist;
  //   largestDirection = 3;
  // } else if (backDist > largestDist) {
  //   largestDist = backDist;
  //   largestDirection = 4;
  // } else if (backLeftDist > largestDist) {
  //   largestDist = backLeftDist;
  //   largestDirection = 5;
  // } else if (leftDist > largestDist) {
  //   largestDist = leftDist;
  //   largestDirection = 6;
  // } else if (frontLeftDist > largestDist) {
  //   largestDist = frontLeftDist;
  //   largestDirection = 7;
  // }

  // if (largestDist > goDist) {
  //   switch (largestDirection) {
  //       case 0: // go front
  //         directions[2] += reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[3] -= reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         break;
  //       case 1: // go front right
  //         directions[2] += reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[3] -= reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[0] += reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[1] -= reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         break;
  //       case 2: // go right
  //         directions[0] += reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[1] -= reactionSpeed * (largestDist - goDist) / (512 - goDist);   
  //         break;
  //       case 3: // go back right
  //         directions[2] -= reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[3] += reactionSpeed * (largestDist - goDist) / (512 - goDist);  
  //         directions[0] += reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[1] -= reactionSpeed * (largestDist - goDist) / (512 - goDist); 
  //         break;
  //       case 4: // go back
  //         directions[2] -= reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[3] += reactionSpeed * (largestDist - goDist) / (512 - goDist);   
  //         break;
  //       case 5: // go back left
  //         directions[2] -= reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[3] += reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[0] -= reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[1] += reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         break;
  //       case 6: // go left
  //         directions[0] -= reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[1] += reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         break;
  //       case 7: // go front left
  //         directions[2] -= reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[3] += reactionSpeed * (largestDist - goDist) / (512 - goDist);  
  //         directions[0] += reactionSpeed * (largestDist - goDist) / (512 - goDist);
  //         directions[1] -= reactionSpeed * (largestDist - goDist) / (512 - goDist); 
  //         break;
  //       default:
  //         break;
  //   }
  // }

  if (frontDist < moveDist) {
    directions[2] -= reactionSpeed * (moveDist - frontDist) / moveDist;
    directions[3] += reactionSpeed * (moveDist - frontDist) / moveDist;
  }

  if (backDist < moveDist) {
    directions[2] += reactionSpeed * (moveDist - backDist) / moveDist;
    directions[3] -= reactionSpeed * (moveDist - backDist) / moveDist;
  }

  if (leftDist < moveDist) {
    directions[0] += reactionSpeed * (moveDist - leftDist) / moveDist;
    directions[1] -= reactionSpeed * (moveDist - leftDist) / moveDist;
  }

  if (rightDist < moveDist) {
    directions[0] -= reactionSpeed * (moveDist - rightDist) / moveDist;
    directions[1] += reactionSpeed * (moveDist - rightDist) / moveDist;
  }

  if (frontLeftDist < moveDist) {
    directions[2] -= reactionSpeed * (moveDist - frontLeftDist) / moveDist;
    directions[3] += reactionSpeed * (moveDist - frontLeftDist) / moveDist;
    directions[0] += reactionSpeed * (moveDist - frontLeftDist) / moveDist;
    directions[1] -= reactionSpeed * (moveDist - frontLeftDist) / moveDist;
  }

  if (frontRightDist < moveDist) {
    directions[2] -= reactionSpeed * (moveDist - frontRightDist) / moveDist;
    directions[3] += reactionSpeed * (moveDist - frontRightDist) / moveDist;
    directions[0] -= reactionSpeed * (moveDist - frontRightDist) / moveDist;
    directions[1] += reactionSpeed * (moveDist - frontRightDist) / moveDist;
  }
    
  if (backLeftDist < moveDist) {
    directions[2] += reactionSpeed * (moveDist - backLeftDist) / moveDist;
    directions[3] -= reactionSpeed * (moveDist - backLeftDist) / moveDist;
    directions[0] += reactionSpeed * (moveDist - backLeftDist) / moveDist;
    directions[1] -= reactionSpeed * (moveDist - backLeftDist) / moveDist;
  }

  if (backRightDist < moveDist) {
    directions[2] += reactionSpeed * (moveDist - backRightDist) / moveDist;
    directions[3] -= reactionSpeed * (moveDist - backRightDist) / moveDist;
    directions[0] -= reactionSpeed * (moveDist - backRightDist) / moveDist;
    directions[1] += reactionSpeed * (moveDist - backRightDist) / moveDist;
  }
    
  // set motor direction and speed
  for (int i = 0; i < 4; i++) {
    if (directions[i] > 100) {
      directions[i] = 100;
    } else if (directions[i] < -100) {
      directions[i] = -100;
    }
  }
  motorSpeedDirectionSetFBLR(directions[0], directions[1], directions[2], directions[3]);
  for (int i = 0; i < friction; i++) {
    if (directions[i] > friction) {
      directions[i] -= friction;
    } else if (directions[i] < -friction) {
      directions[i] += friction;
    }
  }
}
