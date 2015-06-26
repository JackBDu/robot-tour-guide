#include <Wire.h>           // for I2C
#include <Ultrasonic.h>     // for ultrasonic ranger
#include "ITG3200.h"        // for gyroscope

#define MotorSpeedSet             0x82
#define DirectionSet              0xaa
#define Nothing                   0x01    // place holder
#define I2CMotorDriverFB       0x0f    // the address of front-back I2CMotorDriver
#define I2CMotorDriverLR       0x0a    // the address of left-right I2CMotorDriver

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
//motorSpeedA : the DC motor A speed; should be 0~100;
//motorSpeedB: the DC motor B speed; should be 0~100;

void MotorSpeedSetFB(unsigned char MotorSpeedA, unsigned char MotorSpeedB)  {
  MotorSpeedA = map(MotorSpeedA, 0, 100, 0, 255);
  MotorSpeedB = map(MotorSpeedB, 0, 100, 0, 255);
  Wire.beginTransmission(I2CMotorDriverFB);  // transmit to device I2CMotorDriverAdd
  Wire.write(MotorSpeedSet);                    // set pwm header 
  Wire.write(MotorSpeedA);                      // send pwma 
  Wire.write(MotorSpeedB);                      // send pwmb    
  Wire.endTransmission();                       // stop transmitting
}

void MotorSpeedSetLR(unsigned char MotorSpeedA, unsigned char MotorSpeedB)  {
  MotorSpeedA = map(MotorSpeedA, 0, 100, 0, 255);
  MotorSpeedB = map(MotorSpeedB, 0, 100, 0, 255);
  Wire.beginTransmission(I2CMotorDriverLR);  // transmit to device I2CMotorDriverAdd
  Wire.write(MotorSpeedSet);                    // set pwm header 
  Wire.write(MotorSpeedA);                      // send pwma 
  Wire.write(MotorSpeedB);                      // send pwmb    
  Wire.endTransmission();                       // stop transmitting
}

//set the direction of DC motor. 
void MotorDirectionSetFB(unsigned char DirectionFB)  {      //  Adjust the direction of the motors 0b0000 I4 I3 I2 I1
  Wire.beginTransmission(I2CMotorDriverFB);              // transmit to device I2CMotorDriverAdd
  Wire.write(DirectionSet);                                 // Direction control header
  Wire.write(DirectionFB);                                  // send direction control information
  Wire.write(Nothing);                                      // need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();                                   // stop transmitting 
}

//set the direction of DC motor. 
void MotorDirectionSetLR(unsigned char DirectionLR)  {      //  Adjust the direction of the motors 0b0000 I4 I3 I2 I1
  Wire.beginTransmission(I2CMotorDriverLR);              // transmit to device I2CMotorDriverAdd
  Wire.write(DirectionSet);                                 // Direction control header
  Wire.write(DirectionLR);                                  // send direction control information
  Wire.write(Nothing);                                      // need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();                                   // stop transmitting 
}

void moveLeft() {
  MotorDirectionSetFB(0b0110);  //0b1010  Rotating in the positive direction 
}

void moveRight() {
  MotorDirectionSetFB(0b1001);  //0b1010  Rotating in the positive direction 
}

void pauseLR() {
  MotorDirectionSetFB(0b0000);  //0b1010  Rotating in the positive direction 
}

void rotateLR() {
  MotorDirectionSetFB(0b0101);
}

void moveBack() {
  MotorDirectionSetLR(0b1001);  //0b1010  Rotating in the positive direction 
}

void moveFront() {
  MotorDirectionSetLR(0b0110);  //0b1010  Rotating in the positive direction 
}

void pauseFB() {
  MotorDirectionSetLR(0b0000);  //0b1010  Rotating in the positive direction 
}

void rotateFB() {
  MotorDirectionSetLR(0b0101);
}

// void MotorDriectionAndSpeedSet(unsigned char Direction,unsigned char MotorSpeedA,unsigned char MotorSpeedB) {  //you can adjust the driection and speed together
//   MotorDirectionSet(Direction);
//   MotorSpeedSetAB(MotorSpeedA,MotorSpeedB);  
// }

void setup()  {
  Wire.begin();                 // join i2c bus (address optional for master)
  gyro.init();
  gyro.zeroCalibrate(200, 10);  //sample 200 times to calibrate and it will take 200*10ms
  delayMicroseconds(10000);     //wait for motor driver to initialization
  Serial.begin(9600);
  MotorSpeedSetFB(100, 100);
  MotorSpeedSetLR(100, 100);
  delay(100);                   //this delay needed
}

void loop()  {
  ultrasonicFront.MeasureInCentimeters();
  ultrasonicBack.MeasureInCentimeters();
  if (ultrasonicFront.RangeInCentimeters < 40 && ultrasonicBack.RangeInCentimeters < 40) {
    rotateFB();
  } else if (ultrasonicFront.RangeInCentimeters < 40) {
    moveBack();
  } else if (ultrasonicBack.RangeInCentimeters < 40) {
    moveFront();
  }
  
  ultrasonicLeft.MeasureInCentimeters();
  ultrasonicRight.MeasureInCentimeters();
  if (ultrasonicLeft.RangeInCentimeters < 40 && ultrasonicRight.RangeInCentimeters < 40) {
    rotateLR();
  } else if (ultrasonicLeft.RangeInCentimeters < 40) {
    moveRight();
  } else if (ultrasonicRight.RangeInCentimeters < 40) {
    moveLeft();
  }
  gyro.getAngularVelocity(&ax,&ay,&az);
  Serial.println(az);
  
  ultrasonicFrontLeft.MeasureInCentimeters();
  if (ultrasonicFrontLeft.RangeInCentimeters < 40) {
    moveBack();
    moveRight();
  }
  
  ultrasonicFrontRight.MeasureInCentimeters();
  if (ultrasonicFrontRight.RangeInCentimeters < 40) {
    moveLeft();
    moveBack();
  }
  
  ultrasonicBackLeft.MeasureInCentimeters();
  if (ultrasonicBackLeft.RangeInCentimeters < 40) {
    moveRight();
    moveFront();    
  }
  
  ultrasonicBackRight.MeasureInCentimeters();
  if (ultrasonicBackRight.RangeInCentimeters < 40) {
    moveLeft();
    moveFront();   
  }
}
