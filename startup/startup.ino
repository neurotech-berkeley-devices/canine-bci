#include <Wire.h>
// Arduino I2C Scanner
// Re-writed by Arbi Abdul Jabbaar
// Using Arduino IDE 1.8.7
// Using GY-87 module for the target
// Tested on 10 September 2019
// This sketch tests the standard 7-bit addresses
// Devices with higher bit address might not be seen properly.

void write(byte reg, int data, int ADDR = 0x64) {
  Wire.beginTransmission(ADDR);   // Send SLAVE ADDR + WRITE bit
  Wire.write(byte(reg));          // Send requested REGISTER ADDR
  Wire.write(byte(data >> 8));    // Send data[15:8]
  Wire.write(byte(data & (255))); // Send data[7:0]
  Wire.endTransmission(ADDR);     // End transmission
}

int read(byte reg, int bytes = 2, int ADDR = 0x64) {
  Wire.beginTransmission(ADDR);   // Send SLAVE ADDR + WRITE bit
  Wire.write(byte(reg));          // Send requested REGISTER ADDR
  Wire.endTransmission(ADDR);     // End transmission

  Wire.requestFrom(ADDR, bytes, true); // Request bytes from I2C

  int index = 0;
  int data = 0;
  while (Wire.available()) {
    byte result = Wire.read();
    data = (data << (8 * index)) + result; // Write data to from I2C to value
    index += 1;
  }
  
  return data;
}

int startup() {
    Wire.begin();     
    write(0x10, 0x1); // Write 0x10 <= 0x1 to set WRITE mode

    int clk_config = read(0x4B, 2);     // Read CLK CONFIG register
    clk_config = clk_config | (1 << 7); // Flip the 7th bit
    write(0x4B, clk_config);            // Write back new value to register

    return !((read(0x10) == 1) && (read(0x4B, 2) == 0x2692));
}

void setup()
{
    Serial.begin(9600); // The baudrate of Serial monitor is set in 9600
    while (!Serial);    // Waiting for Serial Monitor
    Serial.println("\nI2C Scanner");

    int fail = startup();
    if (!fail) {
      Serial.println("SUCCESS");
    } else {
      Serial.println("FAIL");
    }
}

void loop()
{

}
