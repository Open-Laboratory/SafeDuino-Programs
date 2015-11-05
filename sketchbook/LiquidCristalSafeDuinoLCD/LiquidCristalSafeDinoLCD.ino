/*
  LiquidCrystal Library - Hello World
 
 Demonstrates the use a 8x2 LCD display.
 
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

// For Arduino
//LiquidCrystal lcd(13, 12, 11, 10, 8, 9);

// For SafeDuino MEGA 2560
LiquidCrystal lcd(13, 12, 11, 10, 8, 9);
void setup() {
  
  // LCD read/write (SafeDuino only!)
  //pinMode(2, OUTPUT);	
  //digitalWrite(2, LOW);

  // set up the LCD's number of columns and rows: 
  lcd.begin(8, 2);
  // Print a message to the LCD.
  lcd.print("Hello, ");
  lcd.setCursor(0, 1);
  lcd.print("world!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(7, 1);
  // print the number of seconds since reset:
  lcd.print(millis()/1000);
}

