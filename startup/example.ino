#include <Wire.h>


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

  Wire.requestFrom(ADDR, bytes, true); // Request bytes from I2C - SR + SLAVE ADDR + READ

  int index = 0;
  int data = 0;
  while (Wire.available()) {      // Will run twice and pull two bytes
    byte result = Wire.read();
    data = (data << (8 * index)) + result;  // Write data to from I2C to value
    index += 1;
  }
  
  return data;
}


int startup() {
    Wire.begin();     
    write(0x10, 0x1);   // Write 0x10 <= 0x1 to set WRITE mode

    int clk_config = read(0x4B);          // Read CLK CONFIG register
    clk_config = clk_config | (1 << 7);   // Set the 7th bit to one
    write(0x4B, clk_config);              // Write back new value to register

    // Setting slot configurations
    write(0x30, 0x0319);  // SLOTA_LEDMODE
    write(0x35, 0x0319);  // SLOTB_LEDMODE
    write(0x39, 0x21F3);  // SLOTA_AFEMODE
    write(0x3B, 0x21F3);  // SLOTB_AFEMODE
    write(0x31, 0x0813);  // SLOTA_NUMPULSES
    write(0x36, 0x0813);  // SLOTB_NUMPULSES
    write(0x42, 0x1C36);  // SLOTA_GAIN
    write(0x44, 0x1C36);  // SLOTB_GAIN

    // Setting slot offsets
    write(0x18, 0x3FFF);  // SLOTA_CH1_OFFSET
    write(0x19, 0x3FFF);  // SLOTA_CH2_OFFSET
    write(0x1A, 0x1FF0);  // SLOTA_CH3_OFFSET
    write(0x1B, 0x1FF0);  // SLOTA_CH4_OFFSET
    write(0x1E, 0x3FFF);  // SLOTB_CH1_OFFSET
    write(0x1F, 0x3FFF);  // SLOTB_CH2_OFFSET
    write(0x20, 0x1FF0);  // SLOTB_CH3_OFFSET
    write(0x21, 0x1FF0);  // SLOTB_CH4_OFFSET

    // Setting LED settings
    write(0x23, 0x3005);  // ILED1_COURSE -> scale 100% and driver current = 100mA
    write(0x24, 0x3007);  // ILED2_COURSE -> scale 100% and driver current = 130mA
    write(0x25, 0x0207);  // ILED_FINE

    // Setting other values
    write(0x11, 0x30A9);  // SLOT_EN
    write(0x12, 0x000A);  // F_SAMPLE -> 800Hz
    write(0x14, 0x0116);  // PD_LED_SELECT
    write(0x15, 0x0330);  // NUM_AVG - averaging factor can increase to reduce speed of FIFO writing
    write(0x4E, 0x0040);  // ADC_TIMING

    //==============================================
    
    // Setting FIFO interrupt threshold value
    int registerThreshold = read(0x06);
    write(0x06, registerThreshold & (0xC1FF));

    // Writing to FIFO_INT_MASK
    int fifoMask = read(0x01);
    write(0x01, fifoMask & (0xFEFF));

    // INT_IO_CTL
    write(0x02, 0x0006);

    // Write 0x10 <= 0x2 to set NORMAL OPERATION mode
    write(0x10, 0x2);

    return !((read(0x10) == 2) && (read(0x4B) == 0x2692));    // 0 return means success
}


void standby(){
  write(0x10, 0x0001);
  write(0x00, 0x00FF);
  
  write(0x5F, (0xFFFF << 1));
}


void setup()
{
    Serial.begin(9600); // The baudrate of Serial monitor is set in 9600
    while (!Serial);    // Waiting for Serial Monitor
    Serial.println("\nI2C Scanner");

    int success = startup();
    // Serial.println(success);

    write(0x10, 0x0000);
}


void readFIFO()
{
    // Writing to FIFO_ACCESS_ENA
    write(0x5F, 0x1);
    write(0x5F, 0x1);

    delay(100);

    // Test reading data
    int data1 = read(0x60, 4);
    int data2 = read(0x60, 4);

    Serial.println(data1);

    // Writing to FIFO_ACCESS_ENA
    write(0x5F, 0x0);
}


void loop()
{
  /*int data = digitalRead(2);
  if(data){
    readFIFO();
  }
  delay(500);*/
}