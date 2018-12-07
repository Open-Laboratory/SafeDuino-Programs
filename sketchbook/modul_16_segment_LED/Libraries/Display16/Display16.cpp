#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Display16.h"
#include "Print.h"
#include "../SPI/SPI.h"

namespace Font {
static const int symbols[] = {
   '0' , a | b | c | d | e | f | g | h | t | n,
   '1' , n | c | d,
   '2' , a | b | c | u | p | g | f | e,
   '3' , a | b | c | d | e | f | p,
   '4' , c | d | h | u | p,
   '5' , a | b | h | u | p | d | e | f,
   '6' , b | a | h | g | f | e | d | p | u,
   '7' , a | b | c | d ,
   '8' , a | b | c | d | e | f | g | h | u | p,
   '9' , p | u | h | a | b | c | d | e | f,
   '+' , u | p | m | s,
   '-' , u | p,
    L'А', a | b | h | c | u | p | g | d, 
    L'Б', a | b | k | u | p | g | d | f | e, 
    L'В', a | b | m | c | p | s | d | f | e, 
    L'Г', a | b | h | g, 
    L'Д', a | b | m | c | s | d | f | e, 
    L'Е', a | b | h | u | g | f | e, 
    L'Ё', a | b | h | u | g | f | e, 
    L'Ж', k | m | n | t | s | r, 
    L'З', a | b | n | r | f | e, 
    L'И', h | g | t | n | c | d, 
    L'Й', h | g | t | n | c | d | b, 
    L'К', h | g | u | n | r, 
    L'Л', t | m | b | c | d, 
    L'М', h | g | k | n | c | d, 
    L'Н', h | g | u | p | c | d, 
    L'О', a | b | c | d | e | f | g | h, 
    L'П', a | b | c | d | g | h, 
    L'Р', a | b | c | p | u | h | g, 
    L'С', a | b | h | g | f | e, 
    L'Т', a | b | m | s, 
    L'У', k | n | t, 
    L'Ф' , a | b | h | m | c | u | p | s, 
    L'Х', k | n | t | r, 
    L'Ц', h | g | f | e | r | d | c, 
    L'Ч', k | p | c | d, 
    L'Ш', h | g | m | s | c | d | f | e, 
    L'Щ', h | g | m | s | c | d | f | e | r, 
    L'Ъ', a | m | s | p | e | d, 
    L'Ы', h | g | u | s | f | c | d, 
    L'Ь', m | s | p | e | d, 
    L'Э', a | b | c | d | u | p | f | e, 
    L'Ю', h | g | u | n | r | c | d, 
    L'Я', h | a | b | c | u | p | d | t,   

    'A', a | b | h | c | u | p | g | d, 
    'B', a | b | m | c | p | s | d | f | e, 
	'C', a | b | h | g | f | e, 
	'D', a | b | m | c | s | d | f | e,
	'E', a | b | h | u | g | f | e, 
	'F', b | a | h | g | u,
	'G', b | a | h | g | f | e | d | p,
	'H', h | g | u | p | c | d,
	'I', a | b | m | s | f | e,
	'J', b | c | d | e | f,
	'K', h | g | u | n | r, 
	'L', h | g | f | e,
	'M', h | g | k | n | c | d,
	'N', g | h | k | r | d | c,
	'O', a | b | c | d | e | f | g | h,
	'P', a | b | c | p | u | h | g,
	'Q', a | b | c | d | e | f | g | h | r,
	'R', a | b | c | p | u | h | g | r,
	'S', b | a | h | u | r | e | f,
	'T', a | b | m | s,
	'U', c | d | e | f | g | h,
	'V', h | g | t | n,
	'W', h | g | c | d | t | r,
	'X', k | n | t | r, 
	'Y', k | n | s,
	'Z', a | b | n | t | f | e,
	'/',  t | n,
	'\\', k | r,
	',',  u | t,
	'(', n | r,
	')', k | t,
	'=', u | p | f | e,
	'*', k | r | n | t | u | p,
	'|', m | s,
	'?', a | b | c | p | s,
	':' , a | f,
	';' , a | u | t,	
   0 , 0 
};
};

  void Display16::select_display() {
    digitalWrite(csPin, LOW);
  }
  
  void Display16::deselect_display() {
    digitalWrite(csPin, HIGH);
  }
  
   int s_code(int c, const int* symbols) {
    int data = 0;
    for(int i=0; symbols[i] != 0; i+=2) {
      if (symbols[i] == c) {
        data = symbols[i+1];
        break;
      }
    }
    return data;
  } 
  
  int Display16::s_code(int c) {
    int data = 0;
	if (symbols) data = ::s_code(c, symbols);
	if (data == 0)
		data = ::s_code(c, Font::symbols);
    return data;
  }  
  
    Display16::Display16(int cs, int elem, const int* symbols): csPin(cs), elements(elem), symbols(symbols), data_size(elem*5), pos(0), s1(0), s2(0) {
      display_data = (char*)malloc(data_size);
	  clear();
    }
	
	Display16::~Display16() {
	 free(display_data);
	}
    
    void Display16::begin(SPIClass& spi) {
      pinMode(csPin, OUTPUT);
      deselect_display();
      for (int i=0;i<data_size; ++i)
        display_data[i] = 0xFF;      
      spi.begin(); // Настройка SPI    
      updateAll(spi);    
    }
    
    void Display16::update() {
      // обновить один байт 
    }
    
    void Display16::updateAll(SPIClass& spi) {
      select_display();
      for (int i=data_size-1;i>-1; --i)
        spi.transfer(display_data[i]);      
      deselect_display();
      // обновить все байты
    }
    
    char* Display16::readDisplay_data() {
        return display_data;
    }
    
    void Display16::clear() {
      for (int i=0;i<data_size; ++i)
        display_data[i] = 0xFF;  
      pos = 0;      
    }
    
    void Display16::setCursor(int p) {
      pos = p;
    }
    
    void Display16::put(int index, int data) {
      data = ~ data;
      int i = index * 2 + index / 2;
      display_data[i] = data & 0xFF;
      display_data[i+1] = (data >> 8) & 0xFF;
    }
    
    void Display16::putDP(int index, char DP) {
      // DP==0 нет точки
      int i = 4 + (index / 2) * 5;
      if (index & 1)
        if (DP) display_data[i] &= ~0x82; else display_data[i] |= 0x82;
      else
        if (DP) display_data[i] &= ~0x41; else display_data[i] |= 0x41;
    }
    
    char Display16::getDP(int index) {
      int i = 4 + (index / 2) * 5;
      if (index & 1)
        return ~display_data[i] & 0x82;
      else
        return ~display_data[i] & 0x41;      
    }
    
      inline size_t Display16::write(uint8_t value) {
        char s = value;
       
        int S = s;
        
        if ((s1 & 0b11100000) == 0b11000000 ) {
          s2 = s;
          S = ((int)(s1 & 0b11111) << 6) | (s2 & 0b111111); 
          s1 = 0; s2 = 0;         
        }
        
        
        if ((s1 & 0b11110000) == 0b11100000) {
          if (s2) {
            char s3 = s;  
            S = ((int)(s1 & 0b111111) << 12) | ((int)(s2 & 0b111111) << 6) | (s3 & 0b111111);
            s1 = 0; s2 = 0;             
          } else {
            s2 = s;
            return 1;
          }
        }
        
        if ( s1 == 0 && (s & 0b11100000) == 0b11000000 ) {
          s1 = s;
          return 1;
        }
        
        if ( s1 == 0 && (s & 0b11110000) == 0b11100000 ) {
          s1 = s;
          return 1;
        }
        
        if (s == '\r') {
          pos = 0;
          return 1;
        }
        
        if (s == '\n') {
          clear();
          return 1;
        }        
        
        if (s == '.') {
          int p = pos - 1;
          if (p == -1) {
            putDP(0, 1);
            ++pos;
          } else if ( (p >= 0) && (p < elements*2) ) {
            if (getDP(p)) { ++pos; ++p; }
            if (p < elements*2)
              putDP(p, 1);
          }
            
          return 1;
        }          
        
        int code = s_code(S);
        if (pos > -1 && pos < elements*2) {
          put(pos, code);
          putDP(pos, 0);
        }
          
        ++pos;
        return 1; // assume sucess
      }
