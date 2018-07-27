#include <SPI.h>
#define adc_check_errors 9
#define chip_select 10

int bit_data = 24;
int ch_mux = 0; // 0 - AIN0+ AIN1-, 1 - AIN2+ AIN3-
int continuous_mode = 0;

int incomingByte = 0;
const int cmd_buffer_size = 20;
char cmd_buffer[cmd_buffer_size + 1]; // zero plus one
int in_buffer_pos = 0;

void setup() {
 
  // Serial configuration
  Serial.begin(115200); // default
  //Serial.begin(921600); // max speed 921600 for full program
  //Serial.begin(4000000); // max speed 4 000 000 for easy program
  
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
  write_rigister(B01010000, B00010000, B10000000, B00000001); // AIN0+ AIN1-
  //write_rigister(B01010000, B00010000, B10000000, B01000011); // AIN2+ AIN3-
}

void loop() {
  
  // Check errors
  if(!digitalRead(adc_check_errors)) Serial.print("ERROR!");
  
  // reading serial port
  if (Serial.available() > 0) {
    
    incomingByte = Serial.read();
    if (incomingByte == '\n') {
      cmd_buffer[in_buffer_pos] = 0;
      in_buffer_pos = 0;
      analyse_command();
    } else {
      cmd_buffer[in_buffer_pos] = incomingByte;
      in_buffer_pos++;
      if (in_buffer_pos == cmd_buffer_size) in_buffer_pos = 0;
    }
  } 
  
  //digitalWrite(chip_select, LOW);
  //Bit 6 - 1 for read, 0 for write 
  //SPI.transfer(B01000111); // ID register 
  //SPI.transfer(B01000000); // Status register
  //SPI.transfer(B01000010); // Interface mod register
  //SPI.transfer(B01010000); // Channel 0 register
  //SPI.transfer(B01000100); // Data register
  //PI.transfer(B01100000); // Setup config register 0
  //Serial.println(SPI.transfer(0xff),BIN);
  //Serial.println(SPI.transfer(0xff),BIN);
  //digitalWrite(chip_select, HIGH);
  
  if (continuous_mode == 1) {
    read_data();
    Serial.print('\n');
    delay(100);
  }
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
    
    // read register
    digitalWrite(chip_select, LOW);
    SPI.transfer(read_reg);
    byte read_first_byte = SPI.transfer(0xff);
    byte read_second_byte = SPI.transfer(0xff);
    digitalWrite(chip_select, HIGH);
    
    // check of the register
    if((first_byte == read_first_byte) && (second_byte == read_second_byte)) break;
    Serial.println(write_reg, BIN);
    Serial.println(read_first_byte, BIN);
    Serial.println(read_second_byte, BIN);
    Serial.println('\n');
  }
}

void read_data()
{
  digitalWrite(chip_select, LOW);
  SPI.transfer(B01000100); // Data register
  
  float usls = 0;
  if(bit_data == 24) {
    // 24-bit data
    uint8_t byte_data_reg0 = SPI.transfer(0xff);
    uint8_t byte_data_reg1 = SPI.transfer(0xff);
    uint8_t byte_data_reg2 = SPI.transfer(0xff);
    digitalWrite(chip_select, HIGH);
    
    usls = 65536.0*float(byte_data_reg0)+256.0*float(byte_data_reg1)+float(byte_data_reg2); // 24 bit
    usls = 1.31630905e-06*(usls-8388608.0); // 24 Bit Vref - 4.096V
  } else {
    // 16-bit data
    uint8_t byte_data_reg0 = SPI.transfer(0xff);
    uint8_t byte_data_reg1 = SPI.transfer(0xff);
    digitalWrite(chip_select, HIGH);
    
    usls = 256*float(byte_data_reg0)+float(byte_data_reg1); // 16 bit
    usls = 0.0003384555*(usls-32768.0); // 16 Bit Vref - 4.096V
  }
  Serial.print(usls,5);
}

void analyse_command()
{ 
  // read adc
  if ((cmd_buffer[0] == 'R') && (cmd_buffer[1] == 'D')) {
    //Serial.print("RD");
    read_data();
    Serial.print('\n');
    Serial.flush();
    return;
  }
  
  // set bit data
  if ((cmd_buffer[0] == 'B') && (cmd_buffer[1] == 'D')) {
    // set 16-bit data
    if((cmd_buffer[2] == '1') && (cmd_buffer[3] == '6')) {
      bit_data = 16;
      write_rigister(B01000010, B00000010, B00000000, B00000001); // 16 bit mode
      Serial.print("BD16");
      Serial.print('\n');
      Serial.flush();
      return;
    }
    // set 24-bit data
    if ((cmd_buffer[2] == '2') && (cmd_buffer[3] == '4')) {
      bit_data = 24;
      write_rigister(B01000010, B00000010, B00000000, B00000000); // 24 bit mode
      Serial.print("BD24");
      Serial.print('\n');
      Serial.flush();
      return;
    }
    // read bit data
    if(cmd_buffer[2] == '\0') {
      Serial.print("BD");
      Serial.print(bit_data);
      Serial.print('\n');
      Serial.flush();
      return;
    }
    return;
  }
  
  // multiplexer channel selection
  if ((cmd_buffer[0] == 'C') && (cmd_buffer[1] == 'H')) {
    // set channel 0
    if (cmd_buffer[2] == '0') {
      ch_mux = 0;
      write_rigister(B01010000, B00010000, B10000000, B00000001); // AIN0+ AIN1-
      Serial.print("CH0");
      Serial.print('\n');
      Serial.flush();
      return;
    }
    // set channel 1
    if (cmd_buffer[2] == '1') {
      ch_mux = 1;
      write_rigister(B01010000, B00010000, B10000000, B01000011); // AIN2+ AIN3-
      Serial.print("CH1");
      Serial.print('\n');
      Serial.flush();
      return;
    }
    // read selected channel
    if(cmd_buffer[2] == '\0') {
      Serial.print("CH");
      Serial.print(ch_mux);
      Serial.print('\n');
      Serial.flush();
      return;
    }
    return;
  }

  // continuous mode
  if ((cmd_buffer[0] == 'C') && (cmd_buffer[1] == 'M')) {
    if (continuous_mode == 0) {
      continuous_mode = 1;
      Serial.print("CM1\n");
      Serial.flush();
    } else {
      continuous_mode = 0;
      Serial.print("CM0\n");
      Serial.flush();
    }
    return;
  }

  if ((cmd_buffer[0] == 'I') && (cmd_buffer[1] == 'D')) {
    Serial.print("ID_SafeDuino_ad7175-2_test\n");
    Serial.flush();
    return;
  }
}
