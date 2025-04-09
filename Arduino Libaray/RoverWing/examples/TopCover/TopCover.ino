/*
RoverWing TopCover  test

This code is part of RoverWing library: https://github.com/roverwing
It is intended for demonstrating the features of RoverWing Top, an
optional accessory for RoverWing

To use this example, you need to have the following setup:
 - RoverWing board with a Feather board plugged into it
 - A RoverWing Top attached to the RovwerWing+Feather assembly, as described
  in RoverWing User Guide

This example also uses U8G2 graphics library (https://github.com/olikraus/u8g2/);
 before compiling this sketch, make sure that this library is installed
(you can easily do it using Arduino library manager).


Written in 2020 by David Bershadsky, Alexander Kirillov

This example code is in the public domain.
*/
#include <Wire.h>
#include <RoverWing.h>
// Fonts: one (larger) for use when we want to print two lines, the other for three lines
// See https://github.com/olikraus/u8g2/wiki/fntlistall for full list of available fonts
// these definitions need to be BEFORE including RoverWIngTop.h
// They are optional: if you do not use them, default values will be used
//#define TWO_LINE_FONT u8g2_font_helvB14_tr
//#define THREE_LINE_FONT u8g2_font_7x13B_tr
#include <RoverWingTop.h> //library for use with the top display and buttons


//pin numbers for buttons.
//Button A uses pin next to SCL; button B, the next pin
//For feather boards based on Cortex M0, Atmega 328P, or 32u4 chipsets, the pin numbers are
// 5 (button A) and 6 (button B)
//For HUZZAH32-ESP32 Feather, these are pin numbers 14 (button A) and 32 (button B)
#define BUTTON_A 5
#define BUTTON_B 6

Rover r; //this is the name of the rover!
bool blink=true;
void setup() {
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  Wire.begin();
  Wire.setClock(400000); //use fast mode (400 kHz)
  //initialize the top display
  topDisplay.begin();
  topDisplay.clearBuffer();
  //connect to RoverWing
  displayMessage("Welcome to", "Roverwing");
  delay(3000); //wait for 3 second, so that roverwing initializes
  displayMessage("Connecting");
  while (!r.begin() ){
    //if connecting fails, wait and try again...
    delay(200);
  }
  String line2="FW version: "+r.fwVersion();
  String line3="Voltage: "+String(r.getVoltage());
  displayMessage("Connected", line2, line3);
  //configure Neopixels
  r.setPixelCount(3);
  r.setPixelBrightness(64); //1/4 of full brightness - this is already quite bright
  //give user time to read the info
  delay(3000);
  displayMessage("Press  B", "  to continue");
  waitForButton(BUTTON_B);
  displayMessage("Continuing...");
}

void loop(void) {
  //let us blink the three neopixels on top
  //they have indices 1, 2, 3, (counting clockwise)
  if (blink){
      r.setTopLED(GREEN, OFF, GREEN);
  } else {
      r.setTopLED( OFF, RED, OFF);
  }
  blink=!blink;
  delay(250);
  uint16_t time =millis()/1000;//time in seconds
  displayMessage("Time: "+String(time));
}
