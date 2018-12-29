#include <SPI.h>
#define adc_check_errors 9
#define chip_select 10

int bit_data = 16; // 16 bit
int ch_mux = 0; // 0 - AIN0+ AIN1-, 1 - AIN2+ AIN3-
int continuous_mode = 0;

int incomingByte = 0;
const int cmd_buffer_size = 20;
char cmd_buffer[cmd_buffer_size + 1]; // zero plus one
int in_buffer_pos = 0;

unsigned int aver_dev = 1;
unsigned int num_aver = 0;
double sum_usls = 0;
double usls = 0;
unsigned int time_delay = 1000; // us
unsigned int alternating_current = 0;

unsigned long int time_micros_a = 0; // first time of counting
unsigned long int time_micros_b = 0; // end time of counting

void setup() {
  // Serial configuration
  Serial.begin(115200); // default (900 measurements per second max)
  
  // SPI configuration
  SPI.begin(); // initialize SPI, covering MOSI,MISO,SCK signals
  SPI.setBitOrder(MSBFIRST);  // data is clocked in MSB first
  SPI.setDataMode(SPI_MODE3);  // SCLK idle low (CPOL=0), MOSI read on rising edge (CPHI=0)
  //SPI.setClockDivider(SPI_CLOCK_DIV2);  // system clock = 16 MHz, DIV2 - 8 MHz
  pinMode(chip_select,OUTPUT); 
  pinMode(adc_check_errors,INPUT);
  
  // Set ADC mode register
  write_rigister(B01000001, B00000001, B11000000, B00000110); // only Reference 2.5 V
  
  // Set interface mod register
  write_rigister(B01000010, B00000010, B00000000, B00000001); // 16 bit mode default
  //write_rigister(B01000010, B00000010, B00000000, B00000000); // 24 bit mode

  // Set interface mod register and check sum
  //write_rigister(B01000010, B00000010, B00000000, B00100001); // 16 bit mode
  //write_rigister(B01000010, B00000010, B00000000, B00100000); // 24 bit mode

  // Set setup config 0 register
  //write_rigister(B01100000, B00100000, B00010011, B00000000); // Reference 4.096 V default
  write_rigister(B01100000, B00100000, B00010011, B00001000); // Reference 2.5 V
  //write_rigister(B01100000, B00100000, B00010011, B00110000); // // Reference VCC 5 V
  
  // Set channel 0 register
  write_rigister(B01010000, B00010000, B10000000, B00000001); // AIN0+ AIN1- default
  //write_rigister(B01010000, B00010000, B10000000, B01000011); // AIN2+ AIN3-
  
  // Set filter configuration register
  write_rigister(B01101000, B00101000, B00000000, B00000000); // speed 250 000 SPS
  //write_rigister(B01101000, B00101000, B00000000, B00000110); // speed 15 625 SPS
  //write_rigister(B01101000, B00101000, B00000000, B00000111); // speed 10 000 SPS
  //write_rigister(B01101000, B00101000, B00000000, B00001010); // speed 1 000 SPS
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

    //delay(500);
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
  if(bit_data == 24) {
    // 24-bit data
    time_micros_a = micros(); // time count
    //noInterrupts();
    digitalWrite(chip_select, LOW);
    SPI.transfer(B01000100); // Data register
    uint8_t byte_data_reg0 = SPI.transfer(0xff);
    uint8_t byte_data_reg1 = SPI.transfer(0xff);
    uint8_t byte_data_reg2 = SPI.transfer(0xff);
    digitalWrite(chip_select, HIGH);
    //interrupts();
    
    usls = 65536.0*float(byte_data_reg0)+256.0*float(byte_data_reg1)+float(byte_data_reg2); // 24 bit
    //usls = 1.31630905e-06*(usls-8388608.0+28.0); // 24 Bit Vref - 4.096V (+/-11.042 V max)
    //usls = 0.792181858481e-06*(usls-8388608.0); // 24 Bit Vref - 2.5V (+/-6.645 V max)
  } else {
    // 16-bit data
    time_micros_a = micros(); // time count
    //noInterrupts();
    digitalWrite(chip_select, LOW);
    SPI.transfer(B01000100); // Data register
    uint8_t byte_data_reg0 = SPI.transfer(0xff);
    uint8_t byte_data_reg1 = SPI.transfer(0xff);
    digitalWrite(chip_select, HIGH);    
    //noInterrupts();
    
    // checksum register
    /*digitalWrite(chip_select, LOW);
    SPI.transfer(B01000011); // Data register
    uint8_t byte_check_reg0 = SPI.transfer(0xff);
    uint8_t byte_check_reg1 = SPI.transfer(0xff);
    uint8_t byte_check_reg2 = SPI.transfer(0xff);
    digitalWrite(chip_select, HIGH);
    Serial.println(byte_check_reg0,DEC);
    Serial.println(byte_check_reg1,DEC);
    Serial.println(byte_check_reg2,DEC);
    */
    usls = 256*float(byte_data_reg0)+float(byte_data_reg1); // 16 bit
    //usls = 0.0003384555*(usls-32768.0); // 16 Bit Vref - 4.096V (+/-11.042 V max)
    //usls = 0.0002027825626*(usls-32768.0); // 16 Bit Vref - 2.5V (+/-6.645 V max)
  }
  
  if (aver_dev >= 1) {
    num_aver++;
    if((bit_data == 16) && (usls<32768)) usls = 65536 - usls;
    if((bit_data == 24) && (usls<8388608)) usls = 16777216 - usls;
    sum_usls = sum_usls + usls;
  }
  if ((num_aver > aver_dev-1) || (aver_dev == 0)) {
    if (aver_dev) usls = sum_usls/aver_dev;
    
    //usls = 1.31630905e-06*(usls-8388608.0+28.0); // 24 Bit Vref - 4.096V (+/-11.042 V max)
    //usls = 0.792181858481e-06*(usls-8388608.0+207); // 24 Bit Vref - 2.5V (+/-6.645 V max)
    //usls = 0.0003384555*(usls-32768.0); // 16 Bit Vref - 4.096V (+/-11.042 V max)
    //usls = 0.0002027825626*(usls-32768.0); // 16 Bit Vref - 2.5V (+/-6.645 V max)
    //usls = 0.000215475*(usls-32768.0); // 16 Bit Vref - 2.5V (+/-6.645 V max) Vrsm
    //usls = 0.000609457*(usls-32768.0); // 16 Bit Vref - 2.5V (+/-6.645 V max) Vpp
    //usls = -0.00000000000000036230*usls*usls+0.00000238827346570172*usls-20.0091089853813; // 24 Bit Vref - 2.5V (+/-6.645 V max)
    //usls = ((-1.0906659928e-21*usls + 3.3871857475e-14)*usls + 4.3560361461e-6)*usls - 38.280524731; // 24 Bit Vref - 2.5V (+/-15 V max)
    if (bit_data == 16) usls = 0.000391880122951*(usls-32768.0);// divider-0.2, 16 bit,  Vmax +/-12.841 V 
    if (bit_data == 24) usls = 1.53078173028e-6*(usls-8388608.0);// divider-0.2, 24 bit,  Vmax +/-12.841 V 
    
    // time delay
    if (time_delay <= 15) delayMicroseconds(time_delay*1000-100);
    else delay(time_delay);
    
    Serial.print(usls,6);
    Serial.print('\n');
    //Serial.print('\t');
    //Serial.print(micros() - time_micros_b);
    //Serial.print('\n');
    //time_micros_b = micros();
    
    sum_usls = 0;
    num_aver = 0;
    usls = 0;
  }
}

void analyse_command() { 
  // read adc
  if ((cmd_buffer[0] == 'R') && (cmd_buffer[1] == 'D')) {
    read_data();
    Serial.print("RD");
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
    if (cmd_buffer[2]) {
      unsigned int instr = atoi(cmd_buffer + 2);
      if (instr == 0) {
        time_delay = 1000;
        continuous_mode = 0;
        sum_usls = 0;
        num_aver = 0;
        usls = 0;
        Serial.print("CM0\n");
        Serial.flush();
        return;
      }
      if ((instr > 0) && (instr < 2001)) {
        time_delay = instr;
        continuous_mode = 1;
        sum_usls = 0;
        num_aver = 0;
        usls = 0;
        Serial.print("CM");
        Serial.print(instr);
        Serial.print("\n");
        Serial.flush();
      }
      return;
    }
  }
  
  // number average
  // if 1 =< AC =< 10000 then to measured AC current
  // if AC = 0 then to measured DC current
  if ((cmd_buffer[0] == 'A') && (cmd_buffer[1] == 'C')) {
    if (cmd_buffer[2]) {
      unsigned int num = atoi(cmd_buffer + 2);
      if ((num > 0) && (num < 10001)) {
        aver_dev = num;
        Serial.print("AC");
        Serial.print(aver_dev);
        Serial.print("\n");
        Serial.flush();
      } else {
        aver_dev = 0;
        Serial.print("AC0\n");
        Serial.flush();
      }
      return;
    }
  }

  if ((cmd_buffer[0] == 'I') && (cmd_buffer[1] == 'D')) {
    Serial.print("ID_SafeDuino_ad7175-2_test\n");
    Serial.flush();
    return;
  }
}
