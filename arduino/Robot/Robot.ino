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

// void moveLeft() {
//   motorDirectionSetFB(0b0110);  //0b1010  Rotating in the positive direction 
// }

// void moveRight() {
//   motorDirectionSetFB(0b1001);  //0b1010  Rotating in the positive direction 
// }

// void pauseLR() {
//   motorDirectionSetFB(0b0000);  //0b1010  Rotating in the positive direction 
// }

// void rotateLR() {
//   motorDirectionSetFB(0b0101);
// }

// void moveBack() {
//   motorDirectionSetLR(0b1001);  //0b1010  Rotating in the positive direction 
// }

// void moveFront() {
//   motorDirectionSetLR(0b0110);  //0b1010  Rotating in the positive direction 
// }

// void pauseFB() {
//   motorDirectionSetLR(0b0000);  //0b1010  Rotating in the positive direction 
// }

// void rotateFB() {
//   motorDirectionSetLR(0b0101);
// }

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
  // ultrasonicFront.MeasureInCentimeters();
  // ultrasonicBack.MeasureInCentimeters();
  // if (ultrasonicFront.RangeInCentimeters < 40 && ultrasonicBack.RangeInCentimeters < 40) {
  //   rotateFB();
  // } else if (ultrasonicFront.RangeInCentimeters < 40) {
  //   moveBack();
  // } else if (ultrasonicBack.RangeInCentimeters < 40) {
  //   moveFront();
  // }
  
  // ultrasonicLeft.MeasureInCentimeters();
  // ultrasonicRight.MeasureInCentimeters();
  // if (ultrasonicLeft.RangeInCentimeters < 40 && ultrasonicRight.RangeInCentimeters < 40) {
  //   rotateLR();
  // } else if (ultrasonicLeft.RangeInCentimeters < 40) {
  //   moveRight();
  // } else if (ultrasonicRight.RangeInCentimeters < 40) {
  //   moveLeft();
  // }
  // gyro.getAngularVelocity(&ax,&ay,&az);
  // Serial.println(az);
  
  // ultrasonicFrontLeft.MeasureInCentimeters();
  // if (ultrasonicFrontLeft.RangeInCentimeters < 40) {
  //   moveBack();
  //   moveRight();
  // }
  
  // ultrasonicFrontRight.MeasureInCentimeters();
  // if (ultrasonicFrontRight.RangeInCentimeters < 40) {
  //   moveLeft();
  //   moveBack();
  // }
  
  // ultrasonicBackLeft.MeasureInCentimeters();
  // if (ultrasonicBackLeft.RangeInCentimeters < 40) {
  //   moveRight();
  //   moveFront();    
  // }
  
  // ultrasonicBackRight.MeasureInCentimeters();
  // if (ultrasonicBackRight.RangeInCentimeters < 40) {
  //   moveLeft();
  //   moveFront();   
  // }
  while(1) {
    motorSpeedDirectionSetFBLR(100,100,-100,100);
  }
}
