#include <Wire.h>           // for I2C
#include <Ultrasonic.h>     // for ultrasonic ranger
#include <SeeedGrayOLED.h>  // for OCLED display
#include <avr/pgmspace.h>   // for OCLED display

#define MOTOR_SPEED_SET           0x82
#define DIRECTION_SET             0xaa
#define NOTHING                   0x01    // place holder
#define I2C_MOTOR_DRIVER_FB       0x0f    // the address of front-back I2CMotorDriver
#define I2C_MOTOR_DRIVER_LR       0x0a    // the address of left-right I2CMotorDriver

// ultrasonic rangers, numbers specifying digital pins
Ultrasonic ultrasonicLeft(3);
Ultrasonic ultrasonicRight(2);
Ultrasonic ultrasonicFront(5);
Ultrasonic ultrasonicBack(6);
Ultrasonic ultrasonicFrontLeft(8);
Ultrasonic ultrasonicFrontRight(9);
Ultrasonic ultrasonicBackLeft(10);
Ultrasonic ultrasonicBackRight(11);

int rangers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int motors[4] = {0, 0, 0, 0};
char motorLabels[4] = {'F', 'R', 'B', 'L'};

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
void motorSpeedDirectionSetFRBL(char frontMotorSpeed, char rightMotorSpeed, char backMotorSpeed, char leftMotorSpeed) {
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

void setMotors() {
  // normalize the speed values
  for (int i = 0; i < 4; i++) {
    if (motors[i] > 100) {
      motors[i] = 100;
    } else if (motors[i] < -100) {
      motors[i] = -100;
    }
  }
  // set motor direction and speed
  motorSpeedDirectionSetFRBL(motors[0], motors[1], motors[2], motors[3]);
}

// front, frontRight, right, rightBack, back, backLeft, left, frontLeft
void getRangers() {
  ultrasonicFront.MeasureInCentimeters();
  ultrasonicBack.MeasureInCentimeters();
  ultrasonicLeft.MeasureInCentimeters();
  ultrasonicRight.MeasureInCentimeters();
  ultrasonicFrontLeft.MeasureInCentimeters();
  ultrasonicFrontRight.MeasureInCentimeters();
  ultrasonicBackLeft.MeasureInCentimeters();
  ultrasonicBackRight.MeasureInCentimeters();
  rangers[0] = ultrasonicFront.RangeInCentimeters;
  rangers[1] = ultrasonicFrontRight.RangeInCentimeters;
  rangers[2] = ultrasonicRight.RangeInCentimeters;
  rangers[3] = ultrasonicBackRight.RangeInCentimeters;
  rangers[4] = ultrasonicBack.RangeInCentimeters;
  rangers[5] = ultrasonicBackLeft.RangeInCentimeters;
  rangers[6] = ultrasonicLeft.RangeInCentimeters;
  rangers[7] = ultrasonicFrontLeft.RangeInCentimeters;
}

void oledInit() {
  SeeedGrayOled.init();             // initialize SEEED OLED display
  SeeedGrayOled.clearDisplay();     // Clear Display.
  SeeedGrayOled.setNormalDisplay(); // Set Normal Display Mode
  SeeedGrayOled.setVerticalMode();  // Set to vertical mode for displaying text
  SeeedGrayOled.setGrayLevel(15); //Set Grayscale level. Any number between 0 - 15.
}

void oledPrint() {
  for(char i=0; i < 8; i++) {
    SeeedGrayOled.setTextXY(i, 0);
    SeeedGrayOled.putNumber(rangers[i]);
    if (rangers[i] < 100)
      SeeedGrayOled.putString(" ");
    }
    if (rangers[i] < 10)
      SeeedGrayOled.putString(" ");
    }
  }
  // print out motor info
  for(char i=0; i < 4 ; i++) {
    SeeedGrayOled.setTextXY(i, 5);  //set Cursor to ith line, 0th column
    SeeedGrayOled.putChar(motorLabels[i]);
    SeeedGrayOled.putNumber(motors[i]);
    if (motors[i] < 100)
      SeeedGrayOled.putString(" ");
    }
    if (motors[i] < 10)
      SeeedGrayOled.putString(" ");
    }
  }
}

void analyzeData() {
  motors[0] = 100;
  motors[1] = 100;
  motors[2] = 100;
  motors[3] = 100;
}

void setup()  {
  Serial.begin(9600);
  Wire.begin();                 // join i2c bus (address optional for master)
  oledInit();
  delayMicroseconds(10000);     //wait for motor driver to initialization
}

void loop()  {
  getRangers();
  analyzeData();
  setMotors();
  oledPrint();
}
