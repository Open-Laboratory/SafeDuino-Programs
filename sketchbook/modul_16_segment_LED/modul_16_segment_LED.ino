#include <SPI.h>

#include <Display16.h>

namespace Font {
static const int my_font[] = {
    L'π', u|p|t|r,
	L'Δ', f|e|t|r,
	L'λ', k|r|t,	
    0 , 0 
};

};

Display16 display16(8, 3, Font::my_font);

void setup () {
  
  display16.begin(SPI);
  Serial.begin(9600);
}

const char* str = "πΔλСЪЕШЬ ЕЩЁ ЭТИХ МЯГКИХ ФРАНЦУЗСКИХ БУЛОК, ДА ВЫПЕЙ ЧАЮ.";
int pos = 0;
void loop () {
    display16.clear();
    if (str[pos]==0) pos=0;
    display16.print(str + pos);
    display16.updateAll(SPI);
    delay(600);
    if ((str[pos] & 0b11100000) == 0b11000000 ) ++pos;
    if ((str[pos] & 0b11110000) == 0b11100000) pos+= 2;
    pos++;
}

