#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
// For SafeDuino
LiquidCrystal lcd(13, 12, 11, 10, 8, 9);

long int counter_first=0;
long int counter_second=0;

void setup() {
  // вызов функции при срабатывании прервывания от цифрового порта 3
  attachInterrupt(0, blink_first, FALLING);
  attachInterrupt(1, blink_second, FALLING);
  
  // pin for reset couter
  pinMode(A0,INPUT); // reset first counter
  digitalWrite(A0, HIGH);
  pinMode(A1,INPUT); // reset second counter
  digitalWrite(A1, HIGH);

  
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  //lcd.print("  TURN  ");
  lcd.print("1)");
  lcd.setCursor(0, 1);
  lcd.print("2)");
  // Counter 1
  lcd.setCursor(3, 0);
  lcd.print(counter_first);
  // Counter 2
  lcd.setCursor(3, 1);
  lcd.print(counter_second);
}

void loop() {
  if(!digitalRead(A0)) {
    counter_first = 0;
    lcd.setCursor(3, 0);
    lcd.print("0            ");
  }
  if(!digitalRead(A1)) {
    counter_second = 0;
    lcd.setCursor(3, 1);
    lcd.print("0            ");
  }
  delay(100);
}

// Counter one
void blink_first() {
      counter_first++;
      lcd.setCursor(3, 0);
      lcd.print(counter_first);
}

// Counter two
void blink_second() {
      counter_second++;
      lcd.setCursor(3, 1);
      lcd.print(counter_second);
}

