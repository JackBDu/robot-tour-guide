#include <Wire.h>
#include <Ultrasonic.h>

#define MotorSpeedSet             0x82
#define PWMFrequenceSet           0x84
#define DirectionSet              0xaa
#define MotorSetA                 0xa1
#define MotorSetB                 0xa5
#define Nothing                   0x01
#define EnableStepper             0x1a
#define UnenableStepper           0x1b
#define Stepernu                  0x1c
#define I2CMotorDriverAddFB        0x0f   // Set the address of the I2CMotorDriver
#define I2CMotorDriverAddLR        0x0a   // Set the address of the I2CMotorDriver
// set the steps you want, if 255, the stepper will rotate continuely;

char currentDirection = 'F';

Ultrasonic ultrasonicLeft(3);
Ultrasonic ultrasonicRight(4);
Ultrasonic ultrasonicFront(5);
Ultrasonic ultrasonicBack(6);

void SteperStepset(unsigned char stepnu)
{
  Wire.beginTransmission(I2CMotorDriverAddFB); // transmit to device I2CMotorDriverAdd
  Wire.beginTransmission(I2CMotorDriverAddLR); // transmit to device I2CMotorDriverAdd
  Wire.write(Stepernu);          // Send the stepernu command 
  Wire.write(stepnu);            // send the steps
  Wire.write(Nothing);           // send nothing   
  Wire.endTransmission();        // stop transmitting 
}
//////////////////////////////////////////////////////////////////////
//Function to set the 2 DC motor speed
//motorSpeedA : the DC motor A speed; should be 0~100;
//motorSpeedB: the DC motor B speed; should be 0~100;

void MotorSpeedSetAB(unsigned char MotorSpeedA , unsigned char MotorSpeedB)  {
  MotorSpeedA=map(MotorSpeedA,0,100,0,255);
  MotorSpeedB=map(MotorSpeedB,0,100,0,255);
  Wire.beginTransmission(I2CMotorDriverAddFB); // transmit to device I2CMotorDriverAdd
  Wire.write(MotorSpeedSet);        // set pwm header 
  Wire.write(MotorSpeedA);              // send pwma 
  Wire.write(MotorSpeedB);              // send pwmb    
  Wire.endTransmission();    // stop transmitting
  Wire.beginTransmission(I2CMotorDriverAddLR); // transmit to device I2CMotorDriverAdd
  Wire.write(MotorSpeedSet);        // set pwm header 
  Wire.write(MotorSpeedA);              // send pwma 
  Wire.write(MotorSpeedB);              // send pwmb    
  Wire.endTransmission();    // stop transmitting
}

//set the direction of DC motor. 
void MotorDirectionSetFB(unsigned char DirectionFB)  {     //  Adjust the direction of the motors 0b0000 I4 I3 I2 I1
  Wire.beginTransmission(I2CMotorDriverAddFB); // transmit to device I2CMotorDriverAdd
  Wire.write(DirectionSet);        // Direction control header
  Wire.write(DirectionFB);              // send direction control information
  Wire.write(Nothing);              // need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();    // stop transmitting 
}

//set the direction of DC motor. 
void MotorDirectionSetLR(unsigned char DirectionLR)  {     //  Adjust the direction of the motors 0b0000 I4 I3 I2 I1
  Wire.beginTransmission(I2CMotorDriverAddLR); // transmit to device I2CMotorDriverAdd
  Wire.write(DirectionSet);        // Direction control header
  Wire.write(DirectionLR);              // send direction control information
  Wire.write(Nothing);              // need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();    // stop transmitting 
}

void moveLeft() {
  MotorDirectionSetFB(0b0110);  //0b1010  Rotating in the positive direction 
}

void moveRight() {
  MotorDirectionSetFB(0b1001);  //0b1010  Rotating in the positive direction 
}

void moveFront() {
  MotorDirectionSetLR(0b1001);  //0b1010  Rotating in the positive direction 
}

void moveBack() {
  MotorDirectionSetLR(0b0110);  //0b1010  Rotating in the positive direction 
}

//void MotorDriectionAndSpeedSet(unsigned char Direction,unsigned char MotorSpeedA,unsigned char MotorSpeedB)  {  //you can adjust the driection and speed together
//  MotorDirectionSet(Direction);
//  MotorSpeedSetAB(MotorSpeedA,MotorSpeedB);  
//}
void setup()  {
  Wire.begin(); // join i2c bus (address optional for master)
  delayMicroseconds(10000); //wait for motor driver to initialization
  Serial.begin(9600);
  MotorSpeedSetAB(100, 100);
  delay(100); //this delay needed
}
 
void loop()  {
  ultrasonicFront.MeasureInCentimeters();
  ultrasonicLeft.MeasureInCentimeters();
  ultrasonicRight.MeasureInCentimeters();
  ultrasonicBack.MeasureInCentimeters();
//  Serial.println(ultrasonicFront.RangeInCentimeters);
//  Serial.println(ultrasonicLeft.RangeInCentimeters);
//  Serial.println(ultrasonicRight.RangeInCentimeters);
//  Serial.println(ultrasonicBack.RangeInCentimeters);
//  Serial.println();
//  switch (currentDirection) {
//    case 'F':
//      moveFront();
//      break;
//    case 'B':
//      moveBack();
//      break;
//    case 'L':
//      moveLeft();
//      break;
//    case 'R':
//      moveRight();
//      break;
//  }
  if (ultrasonicFront.RangeInCentimeters < 20) {
//    Serial.println("B");
//    currentDirection = 'B';
    moveBack();
  }
  if (ultrasonicBack.RangeInCentimeters < 20) {
//    Serial.println("F");
//    currentDirection = 'F';
    moveFront();
  }
  if (ultrasonicLeft.RangeInCentimeters < 20) {
//    Serial.println("R");
//    currentDirection = 'R';
    moveRight();
  }
  if (ultrasonicRight.RangeInCentimeters < 20) {
//    Serial.println("L");
//    currentDirection = 'L';
    moveLeft();
  }
}
