/*
RoverWing connection test

This code is part of RoverWing library: https://github.com/roverwing

It can be used with any Adafruit Feather board (or compatible) plugged into
RoverWing  board. RoverWing board can be powered through the main power connector,
or it can get power from the Feather. The voltage readings returned by getVoltage()
measure voltage at main power connector; they are useless if RoverWing is
powered by the  Feather board.

Written in 2020 by David Bershadsky, Alexander Kirillov

This example code is in the public domain
*/



#include <Wire.h>
#include <RoverWing.h>

Rover r; //this is the name of the rover!

float v=0; //voltage
bool blink=false;


void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();
  Wire.setClock(400000); //use fast mode (400 kHz)
  Serial.begin(9600); //debugging terminal
  delay(1000); //wait for 1 second, so that roverwing initializes
  // the next several lines could be replaced by just one,
  // r.beginVerbose();

  Serial.print("Connecting to RoverWing");
  while (!r.begin() ){
    //if connecting fails, wait and try again...
    Serial.print(".");
    delay(200);
  }
  Serial.println("");
  Serial.println("Roverwing is connected");
  Serial.println("Firmware version: "+ r.fwVersion());
}
void loop(){
  v=r.getVoltage();
  Serial.print("Voltage: "); Serial.println(v);

  //do the blink
  digitalWrite(LED_BUILTIN, blink);
  blink=!blink;
  delay(500);
}
