#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "Print.h"

namespace Font {
	static const int _  = 0b00000000;
	static const int a  = 0b00000001;
	static const int b  = 0b00000010;
	static const int c  = 0b00000100;
	static const int d  = 0b00001000;
	static const int e  = 0b00010000;
	static const int f  = 0b00100000;
	static const int g  = 0b01000000;
	static const int h  = 0b10000000;

	static const int k  = 0b0000000100000000;
	static const int m  = 0b0000001000000000;
	static const int n  = 0b0000010000000000;
	static const int p  = 0b0000100000000000;
	static const int s  = 0b0001000000000000;
	static const int r  = 0b0010000000000000;
	static const int t  = 0b0100000000000000;
	static const int u  = 0b1000000000000000;
		
};

class SPIClass;

// elements кол-во сжвоенных элементов
class Display16 : public Print {
  const int csPin;
  const int elements;
  const int* symbols;
  const int data_size;
  char* display_data;
  int pos;
  void select_display();  
  void deselect_display();  
  int s_code(int c);
  
  public:
    Display16(int cs, int elem, const int* symbols = 0);	
	~Display16();
    
    void begin(SPIClass& spi);
    
    void update();    
    void updateAll(SPIClass& spi);
    
    void clear();    
    void setCursor(int p);    
    void put(int index, int data);
    
    void putDP(int index, char DP);
    
    char getDP(int index);
    
    char* readDisplay_data();
    
    protected:
      char s1;
      char s2;
      inline size_t write(uint8_t value);
};