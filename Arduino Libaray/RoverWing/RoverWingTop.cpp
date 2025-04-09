#include "RoverWingTop.h"

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C topDisplay(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


void displayMessage(String line1, String line2, String line3){
  char buf[18];
  topDisplay.clearBuffer();
  //convert string to char array
  topDisplay.setFont(THREE_LINE_FONT);
  line1.toCharArray(buf,18);
  topDisplay.drawStr(0,9,buf);
  line2.toCharArray(buf,18);
  topDisplay.drawStr(0,20,buf);
  line3.toCharArray(buf,18);
  topDisplay.drawStr(0,31,buf);
  topDisplay.sendBuffer();
}
void displayMessage(String line1, String line2){
  char buf[14];
  topDisplay.clearBuffer();
  //convert string to char array
  topDisplay.setFont(TWO_LINE_FONT);
  line1.toCharArray(buf,14);
  topDisplay.drawStr(0,15,buf);
  line2.toCharArray(buf,14);
  topDisplay.drawStr(0,31,buf);
  topDisplay.sendBuffer();
}
void displayMessage(String line1){
  char buf[14];
  topDisplay.clearBuffer();
  //convert string to char array
  topDisplay.setFont(TWO_LINE_FONT);
  line1.toCharArray(buf,14);
  topDisplay.drawStr(0,22,buf);
  topDisplay.sendBuffer();
}
void waitForButton(uint8_t pin){
  while(digitalRead(pin)) delay(10);
}
bool waitForButton(uint8_t pin, uint32_t timeout){
  uint32_t endTime=millis()+timeout;
  while(millis()<=endTime)  {
    if (digitalRead(pin)==LOW) return(true);
  }
  return false;
}
