/*
RoverWing GPS  test

This code is part of RoverWing library: https://github.com/roverwing

It can be used with any Adafruit Feather board (or compatible) plugged into
RoverWing  board.

For this test, RoverWing must be  powered through the main power connector,
using 7-14V power source, and a GPS receiver connected to the GPS port.

Written in 2020 by David Bershadsky, Alexander Kirillov

This example code is in the public domain.
*/
#include <Wire.h>
#include <RoverWing.h>

Rover r; //this is the name of the rover!
location_t base; //starting location
float distance;
int  printCounter=0;
bool blink=false;


void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();
  Wire.setClock(400000); //use fast mode (400 kHz)
  Serial.begin(9600); //debugging terminal
  delay(1000); //wait for 1 second, so that roverwing initializes
  //activates RoverWing and prints basic info to Serial
  r.beginVerbose();
  Serial.println("Starting GPS. Please be patient, it can take a while ");
  r.GPSbegin();
  while (r.GPSstatus()!=GPS_OK){
    //still no GPS fix - let's wait more...
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("GPS has location fix!");
  //fetch our location
  r.getGPSlocation();
  //and save it to variable base
  r.saveGPSlocation(base);
}
void loop(){
    r.getGPSlocation(); //update current location
    distance=r.distanceTo(base); //distance in meters
    if (printCounter==0) {
      //time to print header
      Serial.println("Latitude    Longitude     Dist from start (m) ");
      printCounter=20;
    }
    Serial.print(r.latitude(),6); Serial.print("   "); Serial.print(r.longitude(),6);
    Serial.print("     "); Serial.println(distance);
    delay(500);
    //do the blink
    digitalWrite(LED_BUILTIN, blink);
    blink=!blink;
    printCounter--;
}
