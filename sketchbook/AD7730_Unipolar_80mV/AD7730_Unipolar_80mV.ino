#include <Display16.h>

#include <SPI.h>


#define ADC_SINGLE_WRITE 0x00
#define ADC_SINGLE_READ 0x10
#define ADC_START_READ 0x20
#define ADC_STOP_READ 0x30

#define ADC_STATUS_REG 0x00
#define ADC_DATA_REG 0x01
#define ADC_MODE_REG 0x02
#define ADC_FILTER_REG 0x03
#define ADC_DAC_REG 0x04
#define ADC_OFFSET_REG 0x05
#define ADC_GAIN_REG 0x06
#define ADC_TEST_REG 0x07

#define ADC_CONTINUOS_CONVERSION_MODE 0x2000
#define ADC_SINGLE_CONVERSION_MODE 0x4000
#define ADC_INTERNAL_ZERO_SCALE_CALIBRATION 0x8000
#define ADC_INTERNAL_FULL_SCALE_CALIBRATION 0xA000
#define ADC_SYSTEM_ZERO_SCALE_CALIBRATION 0xC000
#define ADC_SYSTEM_FULL_SCALE_CALIBRATION 0xE000
#define ADC_DATA_WORD_LENGTH_16 0x100
#define ADC_DATA_WORD_LENGTH_24 0x000
#define ADC_BURNOUT_CURRENT 0x4
#define ADC_HIREF 0x80
#define ADC_BIPOLAR 0x000
#define ADC_UNIPOLAR 0x1000
#define ADC_RANGE_10 0x00
#define ADC_RANGE_20 0x10
#define ADC_RANGE_40 0x20
#define ADC_RANGE_80 0x30

const int PD4 = 4;
const int PD6 = 6;
const int PB1 = 1;


void ad_wait_rdy() {
  while (digitalRead(8));
}

char ad_check_rdy() {
  return !digitalRead(8);
}

char spi(char data) {
  return SPI.transfer(data);
}

char ad_ReadStatusReg (void) {
  spi(ADC_SINGLE_READ | ADC_STATUS_REG);
  return spi(0);
}

unsigned long ad_ReadGainReg (void) {
  spi(ADC_START_READ | ADC_GAIN_REG);
  unsigned long result = (static_cast<unsigned long>(spi(0)) << 16) & 0xFF0000;
  result |= (static_cast<unsigned long>(spi(0)) << 8) & 0xFF00;
  result |= (static_cast<unsigned long>(spi(0))) & 0xFF;
  spi(ADC_STOP_READ);
  return result;
}

unsigned long ad_ReadOffsetReg (void) {
  spi(ADC_START_READ | ADC_OFFSET_REG);
  unsigned long result = (static_cast<unsigned long>(spi(0)) << 16) & 0xFF0000;
  result |= (static_cast<unsigned long>(spi(0)) << 8) & 0xFF00;
  result |= (static_cast<unsigned long>(spi(0))) & 0xFF;
  spi(ADC_STOP_READ);
  return result;
}

unsigned long ad_ReadTestReg (void) {
  spi(ADC_START_READ | ADC_TEST_REG);
  unsigned long result = (static_cast<unsigned long>(spi(0)) << 16) & 0xFF0000;
  result |= (static_cast<unsigned long>(spi(0)) << 8) & 0xFF00;
  result |= (static_cast<unsigned long>(spi(0))) & 0xFF;
  spi(ADC_STOP_READ);
  return result;
}

unsigned long ad_ReadModeReg (void) {
  spi(ADC_START_READ | ADC_MODE_REG);
  unsigned long result = (static_cast<unsigned long>(spi(0)) << 8) & 0xFF00;
  result |= (static_cast<unsigned long>(spi(0))) & 0xFF;
  spi(ADC_STOP_READ);
  return result;
}

unsigned long ad_ReadFilterReg (void) {
  spi(ADC_START_READ | ADC_FILTER_REG);
  unsigned long result = (static_cast<unsigned long>(spi(0)) << 16) & 0xFF0000;
  result |= (static_cast<unsigned long>(spi(0)) << 8) & 0xFF00;
  result |= (static_cast<unsigned long>(spi(0))) & 0xFF;
  spi(ADC_STOP_READ);
  return result;
}

char ad_ReadDacReg (void) {
  spi(ADC_SINGLE_READ | ADC_DAC_REG);
  return spi(0);
}

void ad_WriteFilterReg (unsigned long mr) {
  spi(ADC_SINGLE_WRITE | ADC_FILTER_REG);
  spi(static_cast<char>((mr >> 16) & 0xFF));
  spi(static_cast<char>((mr >> 8) & 0xFF));
  spi(static_cast<char>(mr & 0xFF));
}
//???
void ad_WriteModeReg (unsigned long mr) {
  spi(ADC_SINGLE_WRITE | ADC_MODE_REG);
  spi(static_cast<char>((mr >> 8) & 0xFF));
  spi(static_cast<char>(mr & 0xFF));
}
//???
void ad_WriteOffsetReg (unsigned long mr) {
  spi(ADC_SINGLE_WRITE | ADC_OFFSET_REG);
  spi(static_cast<char>((mr >> 16) & 0xFF));
  spi(static_cast<char>((mr >> 8) & 0xFF));
  spi(static_cast<char>(mr & 0xFF));
}

//???
void ad_WriteGainReg (unsigned long mr) {
  spi(ADC_SINGLE_WRITE | ADC_GAIN_REG);
  spi(static_cast<char>((mr >> 16) & 0xFF));
  spi(static_cast<char>((mr >> 8) & 0xFF));
  spi(static_cast<char>(mr & 0xFF));
}
//???
void ad_WriteDacReg (char mr) {
  spi(ADC_SINGLE_WRITE | ADC_DAC_REG);
  spi(static_cast<char>(mr & 0xFF));
}

unsigned long ad_ReadDataReg24 (void) {
  spi(ADC_START_READ | ADC_DATA_REG);
  unsigned long result = (static_cast<unsigned long>(spi(0)) << 16) & 0xFF0000;
  result |= (static_cast<unsigned long>(spi(0)) << 8) & 0xFF00;
  result |= (static_cast<unsigned long>(spi(0))) & 0xFF;
  spi(ADC_STOP_READ);
  return result;  
}

unsigned int ad_ReadDataReg16 (void) {
  spi(ADC_START_READ | ADC_DATA_REG);
  unsigned int result = (static_cast<unsigned int>(spi(0)) << 8) & 0xFF00;
  result |= (static_cast<unsigned int>(spi(0))) & 0xFF;
  spi(ADC_STOP_READ);
  return result;  
}

Display16 disp(7,3);

void setup() {
  
  digitalWrite(10, HIGH);
  
  SPI.begin();
  
  
  disp.begin(SPI);
  SPI.setDataMode(SPI_MODE3);
  digitalWrite(10, LOW); 
  
  //digitalWrite(10, LOW);
  // serial
  Serial.begin(115200);

   
  //ad_WriteFilterReg(0x190010);// | 0xF);
  ad_WriteFilterReg(0x1AE010);
  //ad_WriteFilterReg(0x6c010);// | 0xF); 
  //ad_WriteDacReg(0x23); 

  ad_WriteModeReg (
    ADC_INTERNAL_FULL_SCALE_CALIBRATION |
    ADC_HIREF | ADC_RANGE_80| ADC_UNIPOLAR);
  ad_wait_rdy();
 
  ad_WriteModeReg (
    ADC_INTERNAL_ZERO_SCALE_CALIBRATION | 
    ADC_HIREF | ADC_RANGE_80| ADC_UNIPOLAR);
  ad_wait_rdy();

 
  ad_WriteModeReg(
    ADC_CONTINUOS_CONVERSION_MODE |
    ADC_HIREF | ADC_RANGE_80| ADC_UNIPOLAR);// | ADC_UNIPOLAR);
  ad_wait_rdy();
 
  //timer0
  TCCR0A = _BV(WGM01); // CTC
  TCCR0B = _BV(CS01) | _BV(CS00); // clk/64
  OCR0A = 114; // 0,99826 ms
  TIMSK0 = _BV(OCIE0A);
  

}

int incomingByte = 0;
const int cmd_buffer_size = 20;
char cmd_buffer[cmd_buffer_size + 1]; // plus zero
int in_buffer_pos = 0;

// ADC
//! Режим работы ацп, true - непрерывное преобразование
char adc_continuous_mode = 0x00;
char on_adc = 0;

/**
 * Минимально допустимый код АЦП,
 * если измеренный код АЦП меньше устанавливвется флаг перегрузки
 */
unsigned int min_code = 0;
/**
 * Максимально допустимый код АЦП,
 * если измеренный код АЦП больше устанавливвется флаг перегрузки
 */
unsigned int max_code = 0xFFFF;

//! Измеренный код АЦП
unsigned int adc_code = 0x7FFF;

//! Флаг перегрузки
int overloading = 0x00;

void analyse_command();

void loop() {
  if (ad_check_rdy()) {
    adc_code = ad_ReadDataReg24() / 256;
    if ((adc_code > min_code)&&(adc_code < max_code)) {
      overloading = 0;
      DDRD |=  _BV(PD4);
    }
    else {
     overloading = 1;
     DDRD &=  ~(_BV(PD4));
    }
  }
  
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
  

  if (on_adc) {
    if (adc_continuous_mode) {   
        Serial.print("GR");
        Serial.print(adc_code, DEC);
        Serial.print('\n');
        Serial.flush();
        DDRD ^=  _BV(PD6);
      }   
    on_adc = 0;  
    disp.clear();
    disp.print(adc_code, DEC);
    digitalWrite(10, HIGH);
    SPI.setDataMode(SPI_MODE0);
    disp.updateAll(SPI);
    SPI.setDataMode(SPI_MODE3);
    digitalWrite(10, LOW);
  }
  
  
}

unsigned long interval_counter = 0;
unsigned long interval = 1000;

ISR(TIMER0_COMPA_vect) {
  interval_counter++;
  if(interval_counter >= interval) { 
    on_adc = 1;
    interval_counter = 0;
  }
}

void start_continuous_mode(unsigned long intr) {
  if ((intr > 9) && (intr < 1801)) {
    double tm = intr;
    tm = tm*2,170139;
    intr = tm;
    interval = intr;
    adc_continuous_mode = 1;
  }
  return;
}

void stop_continuous_mode() {
  adc_continuous_mode = 0;
}

void analyse_command() {
  
  if ((cmd_buffer[0] == 'G') && (cmd_buffer[1] == 'M')) {
    Serial.print("GM");
    Serial.print(adc_code, DEC);
    Serial.print('\n');
    Serial.flush();
    return;
  }

  if ((cmd_buffer[0] == 'R') && (cmd_buffer[1] == 'C')) {
    char* buf = cmd_buffer;
    start_continuous_mode(atol (cmd_buffer+2));
    return;
  }

  if ((cmd_buffer[0] == 'S') && (cmd_buffer[1] == 'C')) {
    stop_continuous_mode();
    return;
  }

  if ((cmd_buffer[0] == 'S') && (cmd_buffer[1] == 'O')) {
    if (cmd_buffer[2]) {
      char* buf = cmd_buffer + 2;
      char* arg2 = buf;
      while (*buf) {
        if (*buf == ',') {
          *buf = 0;
          arg2 = buf + 1;
        }
        buf++;
      }
    
      min_code = atol (cmd_buffer + 2);
      max_code = atol (arg2);
    }

    Serial.print("SO");
    Serial.print(min_code, DEC);
    Serial.print(',');    
    Serial.print(max_code, DEC);
    Serial.print('\n');
    Serial.flush();
    return;
  }

  if ( (cmd_buffer[0] == 'G') && (cmd_buffer[1] == 'O') ) {
    Serial.print("GO");
    Serial.print(overloading, DEC);
    Serial.print('\n');
    Serial.flush();
    return;
  }

  if ( (cmd_buffer[0] == 'G') && (cmd_buffer[1] == 'S') ) {
    Serial.print("GS");

    if ( ad_ReadStatusReg() & 0x10 ) Serial.print('D');
      else {
        if (adc_continuous_mode) Serial.print('C');
          else Serial.print('W');
      }
    Serial.print('\n');
    Serial.flush();
    return;
  }

  if ( (cmd_buffer[0] == 'I') && (cmd_buffer[1] == 'D') ) {
    Serial.print("ID2f8771d5ebf55e0983210304c6d5197e\n");
    Serial.flush();
    return;
  }
}

