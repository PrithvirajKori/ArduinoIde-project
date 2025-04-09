#include "Arduino.h"
#include "Wire.h"
#include <U8g2lib.h>
// Fonts: one (larger) for use when we want to print two lines, the other for three lines
// See https://github.com/olikraus/u8g2/wiki/fntlistall for full list of available fonts
#ifndef TWO_LINE_FONT
#define TWO_LINE_FONT u8g2_font_helvB14_tr
#endif
#ifndef THREE_LINE_FONT
#define THREE_LINE_FONT u8g2_font_7x13B_tr
#endif

extern U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C topDisplay;
void displayMessage(String line1, String line2, String line3);
void displayMessage(String line1, String line2);
void displayMessage(String line1);
void waitForButton(uint8_t pin);
bool waitForButton(uint8_t pin, uint32_t timeout);
