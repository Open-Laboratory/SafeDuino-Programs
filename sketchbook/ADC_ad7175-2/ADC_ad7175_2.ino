#include <SPI.h>

void setup() {
  // put your setup code here, to run once:
   
  Serial.begin(9600);
  SPI.begin(); // initialize SPI, covering MOSI,MISO,SCK signals
  SPI.setBitOrder(MSBFIRST);  // data is clocked in MSB first
  SPI.setDataMode(SPI_MODE3);  // SCLK idle low (CPOL=0), MOSI read on rising edge (CPHI=0)
  //SPI.setClockDivider(SPI_CLOCK_DIV8);  // system clock = 16 MHz, chip max = 1 MHz
  pinMode(A15,OUTPUT); 
  pinMode(A14,INPUT);
  
  // set register
  digitalWrite(A15, LOW);
  SPI.transfer(B00000010); // Interface mod register
  SPI.transfer(B00000000); // default
  SPI.transfer(B00000001); // 16-bit mode
  //SPI.transfer(B00000000); // 24-bit mode
  digitalWrite(A15, HIGH);
  delay(5);

  // set register
  digitalWrite(A15, LOW);
  SPI.transfer(B00100000); // Setup config 0 register
  SPI.transfer(B00010011); // default
  SPI.transfer(B00000000); // Reference 2.5V (2.507 V)
  digitalWrite(A15, HIGH);
  delay(5);
  
  digitalWrite(A15, LOW);
  SPI.transfer(B00010000); // Channel 0 register
  SPI.transfer(B10000000); // default
  SPI.transfer(B00000001); // default
  digitalWrite(A15, HIGH);
  delay(5);
}

void loop()
{
  // Check errors
  if(!digitalRead(A14)) Serial.print("ERROR!");
  
  digitalWrite(A15, LOW);
  //Bit 6 - 1 for read, 0 for write 
  //SPI.transfer(B01000111); // ID register
  //SPI.transfer(B01000000); // Status register
  //SPI.transfer(B01000010); // Interface mod register
  //SPI.transfer(B01010000); // Channel 0 register
  SPI.transfer(B01000100); // Data register
  //PI.transfer(B01100000); // Setup config register 0
  
  //Serial.println(SPI.transfer(0xff),HEX);
  //Serial.println(SPI.transfer(0xff),HEX);
  
  //Serial.println(SPI.transfer(0xff),BIN);
  //Serial.println(SPI.transfer(0xff),BIN);
  //Serial.println(SPI.transfer(0xff),DEC); // only 24 bit mode*/
  //Serial.println();
  uint8_t byte_data_reg0 = SPI.transfer(0xff);
  uint8_t byte_data_reg1 = SPI.transfer(0xff);
  //uint8_t byte_data_reg2 = SPI.transfer(0xff);
  float usls = 256*float(byte_data_reg0)+float(byte_data_reg1)+1.0;
  //float usls = 65536*float(byte_data_reg0)+256*float(byte_data_reg1)+float(byte_data_reg2)+1.0;
  //float a2 = SPI.transfer(0xff)+1.0;
  
  //Serial.println(7.6507568e-5*(usls-32768),4); // 16 Bit Reference 2.5V (2.507 V)
  //Serial.println(2.9861927e-7*(usls-8388608),6); // 24 Bit Reference 2.5V (2.507 V)
  
  //Serial.println(0.000125305176781*(usls-32768),4); // 16 Bit Reference 4.096V (4.106 V)
  //Serial.println(4.89473342896e-07*(usls-8388608),4); // 16 Bit Reference 4.096V (4.106 V)
  
  Serial.println(0.000336678539*(usls-32768),4); // 16 Bit Reference 4.096V (4.106 V)
  // 62465 -> +10V - 0.00033673435
  // 32768 -> 0 V - 0.000336678539
  // 3080 ->  -10V - 0.00033683643
  //Serial.println(usls);
  
  digitalWrite(A15, HIGH);
  delay(500);
}
