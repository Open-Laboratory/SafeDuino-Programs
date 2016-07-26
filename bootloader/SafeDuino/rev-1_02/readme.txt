// Прошивка atmega328p для SafeDuino MEGA 328 rev1.02
avrdude -B100 -p m328p -F -P /dev/ttyUSB0 -c STK500v2 -U flash:w:ATmegaBOOT_168_atmega328.hex -U lfuse:w:0xFF:m -U hfuse:w:0xDA:m -U efuse:w:0x05:m


// Прошивка atmega16u2 для SafeDuino MEGA 328 rev1.02
avrdude -B100 -p at90usb162 -F -P /dev/ttyUSB0 -c STK500v2 -U flash:w:Arduino-COMBINED-dfu-usbserial-atmega16u2-Uno-Rev3.hex -U lfuse:w:0xFF:m -U hfuse:w:0xD9:m -U efuse:w:0xF4:m -U lock:w:0x0F:m