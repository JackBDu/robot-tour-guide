#include <Wire.h>           // for I2C
#include <Ultrasonic.h>     // for ultrasonic ranger
#include <HMC5883L.h>       // for digital compass
#include <SeeedGrayOLED.h>  // for OCLED display
#include <avr/pgmspace.h>   // for OCLED display

#define MOTOR_SPEED_SET           0x82
#define DIRECTION_SET             0xaa
#define NOTHING                   0x01    // place holder
#define I2C_MOTOR_DRIVER_FB       0x0f    // the address of front-back I2CMotorDriver
#define I2C_MOTOR_DRIVER_LR       0x0a    // the address of left-right I2CMotorDriver

// initialize digital compass
HMC5883L compass;
float declinationAngle = -0.1006;

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
int reactionSpeed = 20;
int obctableDist = 100;
int friction = 5;
int shortestDist = 300;
int shortestDirection = 0;

int moveDirection = 3 * PI/4;

//////////////////////////////////////////////////////////////////////
//Function to set the 2 DC motor speed
// speed should set to 0~100

void motorSpeedSetFB(unsigned char motorSpeedBack, unsigned char motorSpeedFront)  {
  motorSpeedBack = map(motorSpeedBack, 0, 100, 0, 255);
  motorSpeedFront = map(motorSpeedFront, 0, 100, 0, 255);
  Wire.beginTransmission(I2C_MOTOR_DRIVER_FB);  // transmit to device I2CMotorDriverAdd
  Wire.write(MOTOR_SPEED_SET);                    // set pwm header 
  Wire.write(motorSpeedBack);                      // send pwma 
  Wire.write(motorSpeedFront);                      // send pwmb    
  Wire.endTransmission();                       // stop transmitting
}

void motorSpeedSetLR(unsigned char motorSpeedLeft, unsigned char motorSpeedRight)  {
  motorSpeedLeft = map(motorSpeedLeft, 0, 100, 0, 255);
  motorSpeedRight = map(motorSpeedRight, 0, 100, 0, 255);
  Wire.beginTransmission(I2C_MOTOR_DRIVER_LR);  // transmit to device I2CMotorDriverAdd
  Wire.write(MOTOR_SPEED_SET);                    // set pwm header 
  Wire.write(motorSpeedLeft);                      // send pwma 
  Wire.write(motorSpeedRight);                      // send pwmb    
  Wire.endTransmission();                       // stop transmitting
}

// set the direction of DC motor.
// first two is front, second two is back
void motorDirectionSetFB(unsigned char directionFB)  {      //  Adjust the direction of the motors 0b0000 I4 I3 I2 I1
  Wire.beginTransmission(I2C_MOTOR_DRIVER_FB);                 // transmit to device I2CMotorDriverAdd
  Wire.write(DIRECTION_SET);                                 // Direction control header
  Wire.write(directionFB);                                  // send direction control information
  Wire.write(NOTHING);                                      // need to send this byte as the third byte(no meaning)  
  Wire.endTransmission();                                   // stop transmitting 
}

// set the direction of DC motor. 
// first two is right, second two is left
void motorDirectionSetLR(unsigned char directionLR)  {      //  Adjust the direction of the motors 0b0000 I4 I3 I2 I1
  Wire.beginTransmission(I2C_MOTOR_DRIVER_LR);                 // transmit to device I2CMotorDriverAdd
  Wire.write(DIRECTION_SET);                                 // Direction control header
  Wire.write(directionLR);                                  // send direction control information
  Wire.write(NOTHING);                                      // need to send this byte as the third byte(no meaning)  
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

void oledInit() {
  SeeedGrayOled.init();             // initialize SEEED OLED display
  SeeedGrayOled.clearDisplay();     // Clear Display.
  SeeedGrayOled.setNormalDisplay(); // Set Normal Display Mode
  SeeedGrayOled.setVerticalMode();  // Set to vertical mode for displaying text
  SeeedGrayOled.setGrayLevel(15); //Set Grayscale level. Any number between 0 - 15.
}

void avoidObctable(int frontDist, int backDist, int leftDist, int rightDist, int frontLeftDist, int frontRightDist, int backLeftDist, int backRightDist) {

  if (frontDist < obctableDist) {
    directions[2] -= reactionSpeed * (obctableDist - frontDist) / obctableDist;
    directions[3] += reactionSpeed * (obctableDist - frontDist) / obctableDist;
  }

  if (backDist < obctableDist) {
    directions[2] += reactionSpeed * (obctableDist - backDist) / obctableDist;
    directions[3] -= reactionSpeed * (obctableDist - backDist) / obctableDist;
  }

  if (leftDist < obctableDist) {
    directions[0] += reactionSpeed * (obctableDist - leftDist) / obctableDist;
    directions[1] -= reactionSpeed * (obctableDist - leftDist) / obctableDist;
  }

  if (rightDist < obctableDist) {
    directions[0] -= reactionSpeed * (obctableDist - rightDist) / obctableDist;
    directions[1] += reactionSpeed * (obctableDist - rightDist) / obctableDist;
  }

  if (frontLeftDist < obctableDist) {
    directions[2] -= reactionSpeed * (obctableDist - frontLeftDist) / obctableDist;
    directions[3] += reactionSpeed * (obctableDist - frontLeftDist) / obctableDist;
    directions[0] += reactionSpeed * (obctableDist - frontLeftDist) / obctableDist;
    directions[1] -= reactionSpeed * (obctableDist - frontLeftDist) / obctableDist;
  }

  if (frontRightDist < obctableDist) {
    directions[2] -= reactionSpeed * (obctableDist - frontRightDist) / obctableDist;
    directions[3] += reactionSpeed * (obctableDist - frontRightDist) / obctableDist;
    directions[0] -= reactionSpeed * (obctableDist - frontRightDist) / obctableDist;
    directions[1] += reactionSpeed * (obctableDist - frontRightDist) / obctableDist;
  }
    
  if (backLeftDist < obctableDist) {
    directions[2] += reactionSpeed * (obctableDist - backLeftDist) / obctableDist;
    directions[3] -= reactionSpeed * (obctableDist - backLeftDist) / obctableDist;
    directions[0] += reactionSpeed * (obctableDist - backLeftDist) / obctableDist;
    directions[1] -= reactionSpeed * (obctableDist - backLeftDist) / obctableDist;
  }

  if (backRightDist < obctableDist) {
    directions[2] += reactionSpeed * (obctableDist - backRightDist) / obctableDist;
    directions[3] -= reactionSpeed * (obctableDist - backRightDist) / obctableDist;
    directions[0] -= reactionSpeed * (obctableDist - backRightDist) / obctableDist;
    directions[1] += reactionSpeed * (obctableDist - backRightDist) / obctableDist;
  }

}

void setup()  {
  Serial.begin(9600);
  Wire.begin();                 // join i2c bus (address optional for master)
  oledInit();
  compass.setScale(1.3);        // Set the scale of the compass.
  compass.setMeasurementMode(MEASUREMENT_CONTINUOUS); // Set the measurement mode to Continuous
  delayMicroseconds(10000);     //wait for motor driver to initialization
  motorSpeedSetFB(100, 100);
  motorSpeedSetLR(100, 100);
  delay(100);                   //this delay needed
}

void loop()  {
  boolean move = true;
  int shortestDist = 300;
  int shortestDirection = 0;

  ultrasonicFront.MeasureInCentimeters();
  ultrasonicBack.MeasureInCentimeters();
  ultrasonicLeft.MeasureInCentimeters();
  ultrasonicRight.MeasureInCentimeters();
  ultrasonicFrontLeft.MeasureInCentimeters();
  ultrasonicFrontRight.MeasureInCentimeters();
  ultrasonicBackLeft.MeasureInCentimeters();
  ultrasonicBackRight.MeasureInCentimeters();
  int frontDist = ultrasonicFront.RangeInCentimeters;
  if (frontDist < shortestDist) {
    shortestDist = frontDist;
    shortestDirection = 0;
  }
  int backDist = ultrasonicBack.RangeInCentimeters;
  if (backDist < shortestDist) {
    shortestDist = backDist;
    shortestDirection = 1;
  }
  int leftDist = ultrasonicLeft.RangeInCentimeters;
  if (backDist < shortestDist) {
    shortestDist = leftDist;
    shortestDirection = 2;
  }
  int rightDist = ultrasonicRight.RangeInCentimeters;
  if (rightDist < shortestDist) {
    shortestDist = rightDist;
    shortestDirection = 3;
  }
  int frontLeftDist = ultrasonicFrontLeft.RangeInCentimeters;
  if (frontLeftDist < shortestDist) {
    shortestDist = frontLeftDist;
    shortestDirection = 4;
  }
  int frontRightDist = ultrasonicFrontRight.RangeInCentimeters;
  if (frontRightDist < shortestDist) {
    shortestDist = frontRightDist;
    shortestDirection = 5;
  }
  int backLeftDist = ultrasonicBackLeft.RangeInCentimeters;
  if (backLeftDist < shortestDist) {
    shortestDist = backLeftDist;
    shortestDirection = 6;
  }
  int backRightDist = ultrasonicBackRight.RangeInCentimeters;
  if (backRightDist < shortestDist) {
    shortestDist = backRightDist;
    shortestDirection = 7;
  }

  if (shortestDist < 50) {
    move = false;
  }

  float heading;
  // if (move) {
  //   MagnetometerScaled scaled = compass.readScaledAxis();
  //   heading = atan2(scaled.YAxis, scaled.XAxis) + declinationAngle;
  //   // Correct for when signs are reversed.
  //   if(heading < 0)
  //     heading += 2*PI; 
  //   // Check for wrap due to addition of declination.
  //   if(heading > 2*PI)
  //     heading -= 2*PI;

  //   directions[0] += -cos(heading + PI/2 + moveDirection) * reactionSpeed;
  //   directions[1] += cos(heading + PI/2 + moveDirection) * reactionSpeed;
  //   directions[2] += -cos(heading + moveDirection) * reactionSpeed;
  //   directions[3] += cos(heading + moveDirection) * reactionSpeed;
  // }

avoidObctable(frontDist, backDist, leftDist, rightDist, frontLeftDist, frontRightDist, backLeftDist, backRightDist);

  // if (shortestDist < 300) {
  //   if (shortestDirection == 0) {
  //     directions[2] -= reactionSpeed * (300 - frontDist) / 300;
  //     directions[3] += reactionSpeed * (300 - frontDist) / 300;
  //   }

  //   if (shortestDirection == 1) {
  //     directions[2] += reactionSpeed * (300 - backDist) / 300;
  //     directions[3] -= reactionSpeed * (300 - backDist) / 300;
  //   }

  //   if (shortestDirection == 2) {
  //     directions[0] += reactionSpeed * (300 - leftDist) / 300;
  //     directions[1] -= reactionSpeed * (300 - leftDist) / 300;
  //   }

  //   if (shortestDirection == 3) {
  //     directions[0] -= reactionSpeed * (300 - rightDist) / 300;
  //     directions[1] += reactionSpeed * (300 - rightDist) / 300;
  //   }

  //   if (shortestDirection == 4) {
  //     directions[2] -= reactionSpeed * (300 - frontLeftDist) / 300;
  //     directions[3] += reactionSpeed * (300 - frontLeftDist) / 300;
  //     directions[0] += reactionSpeed * (300 - frontLeftDist) / 300;
  //     directions[1] -= reactionSpeed * (300 - frontLeftDist) / 300;
  //   }

  //   if (shortestDirection == 5) {
  //     directions[2] -= reactionSpeed * (300 - frontRightDist) / 300;
  //     directions[3] += reactionSpeed * (300 - frontRightDist) / 300;
  //     directions[0] -= reactionSpeed * (300 - frontRightDist) / 300;
  //     directions[1] += reactionSpeed * (300 - frontRightDist) / 300;
  //   }
      
  //   if (shortestDirection == 6) {
  //     directions[2] += reactionSpeed * (300 - backLeftDist) / 300;
  //     directions[3] -= reactionSpeed * (300 - backLeftDist) / 300;
  //     directions[0] += reactionSpeed * (300 - backLeftDist) / 300;
  //     directions[1] -= reactionSpeed * (300 - backLeftDist) / 300;
  //   }

  //   if (shortestDirection == 7) {
  //     directions[2] += reactionSpeed * (300 - backRightDist) / 300;
  //     directions[3] -= reactionSpeed * (300 - backRightDist) / 300;
  //     directions[0] -= reactionSpeed * (300 - backRightDist) / 300;
  //     directions[1] += reactionSpeed * (300 - backRightDist) / 300;
  //   }
  // }

  // add a bit friction
  // for (int i = 0; i < friction; i++) {
  //   if (directions[i] > friction) {
  //     directions[i] -= friction;
  //   } else if (directions[i] < -friction) {
  //     directions[i] += friction;
  //   }
  // }

  // set motor direction and speed
  for (int i = 0; i < 4; i++) {
    if (directions[i] > 100) {
      directions[i] = 100;
    } else if (directions[i] < -100) {
      directions[i] = -100;
    }
  }
  motorSpeedDirectionSetFBLR(directions[0], directions[1], directions[2], directions[3]);

  // SeeedGrayOled.setTextXY(0,0);  //set Cursor to ith line, 0th column
  // SeeedGrayOled.putString("     ");  
  // SeeedGrayOled.setTextXY(0,0);  //set Cursor to ith line, 0th column
  // SeeedGrayOled.putNumber(heading*57.29);
  for(char i=0; i < 4 ; i++) {
    SeeedGrayOled.setTextXY(i+2,0);  //set Cursor to ith line, 0th column
    SeeedGrayOled.putString("     ");  
    SeeedGrayOled.setTextXY(i+2,0);  //set Cursor to ith line, 0th column
    SeeedGrayOled.putNumber(directions[i]);
  }
}
