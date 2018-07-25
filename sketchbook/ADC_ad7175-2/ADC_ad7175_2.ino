#include <SPI.h>
#define A14 9
#define A15 10

void setup() {
  // put your setup code here, to run once:
   
  Serial.begin(9600); // default 9600 //max speed 4 000 000
  SPI.begin(); // initialize SPI, covering MOSI,MISO,SCK signals
  SPI.setBitOrder(MSBFIRST);  // data is clocked in MSB first
  SPI.setDataMode(SPI_MODE3);  // SCLK idle low (CPOL=0), MOSI read on rising edge (CPHI=0)
  //SPI.setClockDivider(SPI_CLOCK_DIV2);  // system clock = 16 MHz, chip max = 1 MHz
  pinMode(A15,OUTPUT); 
  pinMode(A14,INPUT);
  
  // set register
  digitalWrite(A15, LOW);
  SPI.transfer(B00000010); // Interface mod register
  SPI.transfer(B00000000); // default
  //SPI.transfer(B00000001); // 16-bit mode
  SPI.transfer(B00000000); // 24-bit mode
  digitalWrite(A15, HIGH);
  delay(5);

  // set register
  digitalWrite(A15, LOW);
  SPI.transfer(B00100000); // Setup config 0 register
  SPI.transfer(B00010011); // default
  //SPI.transfer(B00000000); // Reference 2.5V
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
  //Serial.println(SPI.transfer(0xff),DEC); // only 24 bit mode
  //Serial.println();
  uint8_t byte_data_reg0 = SPI.transfer(0xff);
  uint8_t byte_data_reg1 = SPI.transfer(0xff);
  uint8_t byte_data_reg2 = SPI.transfer(0xff); // only 24 bit mode
  
  //float usls = 256*float(byte_data_reg0)+float(byte_data_reg1)+1.0; // 16 bit
  float usls = 65536.0*float(byte_data_reg0)+256.0*float(byte_data_reg1)+float(byte_data_reg2)+1.0; // 24 bit
  
  //usls = 0.03384555*(usls-32768.0); // 16 Bit Vref - 2.5V
  //usls = 1.31630905e-04*(usls-8388608.0); //  24 Bit Vref - 2.5V
  
  //usls = 0.0003384555*(usls-32768.0); // 16 Bit Vref - 4.096V
  usls = 1.31630905e-06*(usls-8388608.0); // 24 Bit Vref - 4.096V
  
  Serial.println(usls,5);
  //Serial.println(usls);
  
  digitalWrite(A15, HIGH);
  delay(500);
}
