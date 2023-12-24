
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
int buttonEncoderD = 0;
int buttonOn = 0;

void setup() {
  Serial.begin(9600); 

  // declare pin 9 to be an output:
  pinMode(7, OUTPUT);         // устанавливаем pin 7 как выход
  pinMode(pin_A, INPUT);
  pinMode(pin_B, INPUT);
  currentTime = millis();
  loopTime = currentTime; 
  
  // Timer 4 init
  cli();
  TCCR4A = 0; // reset bit WGM21, WGM20
  TCCR4B = 0;
  //TCCR4B |= (0<<CS42)|(0<<CS41)|(1<<CS40); // clkI/1
  //TCCR4B |= (0<<CS42)|(1<<CS41)|(0<<CS40); // clkI/8
  //TCCR4B |= (0<<CS42)|(1<<CS41)|(1<<CS40); // clkI/64
  //TCCR4B |= (1<<CS42)|(0<<CS41)|(0<<CS40); // clkI/256
  TCCR4B |= (1<<CS42)|(0<<CS41)|(1<<CS40); // clkI/1024
  TCCR4B |= (1<<WGM42); // прерываение по совпадению c OCR4A
  TIMSK4 |= (1<<OCIE4A);
  OCR4A = 125; // 125 Гц
  sei();
}

void loop() {
  // Работа с энкодером по внутреннему времени МК
  /*currentTime = millis();
  if(currentTime >= (loopTime + 10)){ // проверяем каждые 10мс (100 Гц)
    encoder_A = digitalRead(pin_A);     // считываем состояние выхода А энкодера 
    encoder_B = digitalRead(pin_B);     // считываем состояние выхода B энкодера    
    encoder_D = digitalRead(pin_D);     // считываем состояние выхода D энкодера
    if(encoder_D) buttonEncoderD = 1;
    else {
      if (buttonEncoderD) {
        buttonEncoderD = 0;
        if(buttonOn) {
          buttonOn = 0;
          brightness = 0;
          Serial.println(brightness);
        } else {
          buttonOn = 1;
        }
      }
    }
    if((!encoder_A) && (encoder_A_prev)){    // если состояние изменилось с положительного к нулю
      if(encoder_B) {
        // выход В в полож. сост., значит вращение по часовой стрелке
        // увеличиваем яркость, не более чем до 255
        if(brightness + fadeAmount <= 255) brightness += fadeAmount;
        Serial.println(brightness);
      }   
      else {
        // выход В в 0 сост., значит вращение против часовой стрелки     
        // уменьшаем яркость, но не ниже 0
        if(brightness - fadeAmount >= 0) brightness -= fadeAmount;
        Serial.println(brightness);
      }   
 
    }   
    encoder_A_prev = encoder_A;     // сохраняем значение А для следующего цикла 
    loopTime = currentTime;
  }*/                     
}

// Работа с энкодером на прерывании основанном на переполнении таймера
ISR(TIMER4_COMPA_vect) {
    encoder_A = digitalRead(pin_A);     // считываем состояние выхода А энкодера 
    encoder_B = digitalRead(pin_B);     // считываем состояние выхода B энкодера    
    encoder_D = digitalRead(pin_D);     // считываем состояние выхода D энкодера
    if(encoder_D) buttonEncoderD = 1;
    else {
      if (buttonEncoderD) {
        buttonEncoderD = 0;
        if(buttonOn) {
          buttonOn = 0;
          brightness = 0;
          Serial.println(brightness);
        } else {
          buttonOn = 1;
        }
      }
    }

    if((!encoder_A) && (encoder_A_prev)){    // если состояние изменилось с положительного к нулю
      if(encoder_B) {
        // выход В в полож. сост., значит вращение по часовой стрелке
        // увеличиваем яркость, не более чем до 255
        if(brightness + fadeAmount <= 255) brightness += fadeAmount;
        Serial.println(brightness);
      }   
      else {
        // выход В в 0 сост., значит вращение против часовой стрелки     
        // уменьшаем яркость, но не ниже 0
        if(brightness - fadeAmount >= 0) brightness -= fadeAmount;
        Serial.println(brightness);
      }   
 
    }   
    encoder_A_prev = encoder_A;     // сохраняем значение А для следующего цикла 
}
