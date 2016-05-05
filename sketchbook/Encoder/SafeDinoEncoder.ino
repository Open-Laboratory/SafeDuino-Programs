/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 20x4 LCD display.
 
 This sketch prints "Hello World!" to the LCD
 and shows the time.
 
  The circuit (for SafeDuino):
 * LCD RS pin to digital pin 13
 * LCD Enable pin to digital pin 12
 * LCD D4 pin to digital pin 11
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 8
 * LCD D7 pin to digital pin 9
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins

// For SafeDuino
//LiquidCrystal lcd(13, 12, 11, 10, 8, 9);

// For SafeDuino MEGA 2560
LiquidCrystal lcd(13, 12, 11, 10, 8, 9);

// Encoder
int brightness = 125;       // яркость LED, начинаем с половины
int fadeAmount = 5;        // шаг изменения яркости LED
unsigned long currentTime;
unsigned long loopTime;
const int pin_A = A1;       // pin A2
const int pin_B = A2;       // pin A1
const int pin_D = A0;       // pin A0
unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_D;
unsigned char encoder_A_prev=0;

void setup() {
  // declare pin 9 to be an output:
  pinMode(7, OUTPUT);         // устанавливаем pin 7 как выход
  pinMode(pin_A, INPUT);
  pinMode(pin_B, INPUT);
  currentTime = millis();
  loopTime = currentTime; 
  
  // LCD read/write (SafeDuino only!)
  //pinMode(2, OUTPUT);	
  //digitalWrite(2, LOW);

  // set up the LCD's number of columns and rows: 
  lcd.begin(8, 2);
  // Print a message to the LCD.
  lcd.print("Encoder:");
  lcd.setCursor(0, 1);
  lcd.print("0       ");
}

void loop() {
  currentTime = millis();
  if(currentTime >= (loopTime + 5)){ // проверяем каждые 5мс (200 Гц)
    encoder_A = digitalRead(pin_A);     // считываем состояние выхода А энкодера 
    encoder_B = digitalRead(pin_B);     // считываем состояние выхода B энкодера    
    encoder_D = digitalRead(pin_D);     // считываем состояние выхода D энкодера
    if(!encoder_D) {
      brightness = 0;
      lcd.setCursor(0, 1);
      lcd.print("0       ");
    }
    if((!encoder_A) && (encoder_A_prev)){    // если состояние изменилось с положительного к нулю
      lcd.setCursor(0, 1);
      lcd.print("        ");
      if(encoder_B) {
        // выход В в полож. сост., значит вращение по часовой стрелке
        // увеличиваем яркость, не более чем до 255
        if(brightness + fadeAmount <= 255) brightness += fadeAmount;               
      }   
      else {
        // выход В в 0 сост., значит вращение против часовой стрелки     
        // уменьшаем яркость, но не ниже 0
        if(brightness - fadeAmount >= 0) brightness -= fadeAmount;               
      }   
 
    }   
    encoder_A_prev = encoder_A;     // сохраняем значение А для следующего цикла 
     
    //analogWrite(7, brightness);   // устанавливаем яркость на 9 ножку
    lcd.setCursor(0, 1);
    lcd.print(brightness);
    
    loopTime = currentTime;
  }                       
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  //lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  //lcd.print(millis()/1000);
}

