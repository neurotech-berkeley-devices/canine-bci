# include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Wire.beginTransmission(0x64);
  Wire.write(byte(0x10));
  Wire.write(byte(0x00));
  Wire.write(byte(0x01));
}

void loop() {
  // put your main code here, to run repeatedly:
   
}
