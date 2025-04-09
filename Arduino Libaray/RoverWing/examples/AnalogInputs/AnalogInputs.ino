/*
RoverWing analog inputs test

This code is part of RoverWing library: https://github.com/roverwing

It can be used with any Adafruit Feather board (or compatible) plugged into
RoverWing  board. RoverWing board can be powered through the main power connector,
or it can get power from the Feather. The voltage readings returned by getVoltage()
measure voltage at main power connector; they are useless if RoverWing is
powered by the  Feather board.

IMPORTANT: analog inputs voltage should not exceed 3.3V, otherwise the RoverWing
can be permanently damaged!

Written in 2020 by David Bershadsky, Alexander Kirillov

This example code is in the public domain.
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
  //activates RoverWing and prints basic info to Serial
  r.beginVerbose();
}
void loop(){
  v=r.getVoltage();
  Serial.print("Voltage: "); Serial.println(v);

  //get and print analog inputs

  r.getAllAnalog(); //fetches values from RoverBoard and saves them in r.analog[] array
  Serial.print("Analog inputs:");
  for (int i=1; i<=6; i++){
    Serial.print("  ");
    Serial.print(r.analog[i]); //note that index i runs 1..6, not 0..5 !
                               // values of analog[i] are floats, representing
                               // the voltage; they range 0 - 3.3 volts
  }
  Serial.println(" ");
  delay(500);

  //do the blink
  digitalWrite(LED_BUILTIN, blink);
  blink=!blink;
}
