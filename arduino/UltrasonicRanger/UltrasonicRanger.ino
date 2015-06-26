#include <Ultrasonic.h>

Ultrasonic ultrasonicA(2);
void setup()
{
  Serial.begin(9600);
}
void loop()
{
  ultrasonicA.MeasureInCentimeters();
  Serial.println(ultrasonicA.RangeInCentimeters);
  delay(100);
}
