#include <SPI.h>
#define adc_check_errors 9
#define chip_select 10

void setup() {
   
  // Serial configuration
  Serial.begin(115200); // default
  //Serial.begin(4000000); // max speed 4 000 000
  
  // SPI configuration
  SPI.begin(); // initialize SPI, covering MOSI,MISO,SCK signals
  SPI.setBitOrder(MSBFIRST);  // data is clocked in MSB first
  SPI.setDataMode(SPI_MODE3);  // SCLK idle low (CPOL=0), MOSI read on rising edge (CPHI=0)
  //SPI.setClockDivider(SPI_CLOCK_DIV2);  // system clock = 16 MHz
  pinMode(chip_select,OUTPUT); 
  pinMode(adc_check_errors,INPUT);
  
  // Set interface mod register
  write_rigister(B01000010, B00000010, B00000000, B00000000); // 24 bit mode
  //write_rigister(B01000010, B00000010, B00000000, B00000001); // 16 bit mode

  // Set setup config 0 register
  write_rigister(B01100000, B00100000, B00010011, B00000000); // Reference 4.096 V
  //write_rigister(B01100000, B00100000, B00010011, B00100000); // Reference 2.5 V
  //write_rigister(B01100000, B00100000, B00010011, B00110000); // // Reference VCC 5 V
  
  // Set channel 0 register
  write_rigister(B01010000, B00010000, B10000000, B00000001); // default AIN0+ AIN1-
  //write_rigister(B01010000, B00010000, B10000000, B01000011); // default AIN2+ AIN3-
}

void loop()
{
  // Check errors
  if(!digitalRead(adc_check_errors)) Serial.print("ERROR!");
  
  digitalWrite(chip_select, LOW);
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
  digitalWrite(chip_select, HIGH);
  
  //float usls = 256*float(byte_data_reg0)+float(byte_data_reg1)+1.0; // 16 bit
  float usls = 65536.0*float(byte_data_reg0)+256.0*float(byte_data_reg1)+float(byte_data_reg2)+1.0; // 24 bit
  
  //usls = 0.0003384555*(usls-32768.0); // 16 Bit Vref - 4.096V
  usls = 1.31630905e-06*(usls-8388608.0); // 24 Bit Vref - 4.096V
  
  Serial.println(usls,5);
  
  digitalWrite(chip_select, HIGH);
  delay(500);
}

void write_rigister(byte read_reg, byte write_reg, byte first_byte, byte second_byte)
{
  while(1){
    // write register
    digitalWrite(chip_select, LOW);
    SPI.transfer(write_reg);
    
    SPI.transfer(first_byte);
    SPI.transfer(second_byte);
    digitalWrite(chip_select, HIGH);
    //delay(5);
    
    // read register
    digitalWrite(chip_select, LOW);
    SPI.transfer(read_reg);
    byte read_first_byte = SPI.transfer(0xff);
    byte read_second_byte = SPI.transfer(0xff);
    digitalWrite(chip_select, HIGH);
    //delay(5);
    
    // check of the register
    if((first_byte == read_first_byte) && (second_byte == read_second_byte)) break;
    Serial.println(write_reg, BIN);
    Serial.println(read_first_byte, BIN);
    Serial.println(read_second_byte, BIN);
    Serial.println('\n');
  }
}
