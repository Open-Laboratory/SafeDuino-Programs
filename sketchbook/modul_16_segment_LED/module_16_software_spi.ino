#include <SPI.h>
#include <Display16.h>
#define spi_prog_cs A3
#define spi_prog_mosi A4
#define spi_prog_sck A5

int display16_cs = 8;
int display16_elem = 3;
Display16 display16(display16_cs, display16_elem);
int data_size = display16_elem*5;
char* display_data;

void setup() {
  // SPI software configuration
  pinMode(spi_prog_sck, OUTPUT);
  pinMode(spi_prog_cs, OUTPUT);
  pinMode(spi_prog_mosi, OUTPUT);
  digitalWrite(spi_prog_sck, LOW); 
  digitalWrite(spi_prog_cs, LOW);
  digitalWrite(spi_prog_mosi, LOW);
}

float num_i = 0.001;

void loop() {
  // Очищаем индикатор
  display16_clear();
  
  // Передаём данные на индикатор
  num_i = num_i + 0.001;
  display16.print(num_i,3);
  display_data = display16.readDisplay_data();
  digitalWrite(spi_prog_cs, LOW);
  for (int i=data_size-1;i>-1; --i)
    spi_sendbyte(display_data[i]);
  digitalWrite(spi_prog_cs, HIGH);
  delay(500);
}

// Очищаем индикатор
void display16_clear() {
  display16.clear();
  display_data = display16.readDisplay_data();
  digitalWrite(spi_prog_cs, LOW);
  for (int i=data_size-1;i>-1; --i)
    spi_sendbyte(display_data[i]);
  digitalWrite(spi_prog_cs, HIGH);
}

// Передаём один байт данных по программному SPI
void spi_sendbyte(unsigned char byte_data) {
   for(unsigned char i=0; i<8; i++) {
      // проверить старш бит = 1
      if (byte_data & 0x80) {
         //SPI_SDI = 1; // передать 1
         digitalWrite(spi_prog_mosi, HIGH);
      } else {
         //SPI_SDI = 0; // передать 0
         digitalWrite(spi_prog_mosi, LOW);
      }
      //SPI_SCK = 1; // синхроимпульс
      digitalWrite(spi_prog_sck, HIGH); 
      byte_data <<= 1;     // сдвиг для передачи след бита
      //SPI_SCK = 0; // синхроимпульс
      digitalWrite(spi_prog_sck, LOW); 
   }
}

