/*
RoverWing IMU test

This code is part of RoverWing library: https://github.com/roverwing

It can be used with any Adafruit Feather board (or compatible) plugged into
RoverWing  board. RoverWing board can be powered through the main power connector,
or it can get power from the Feather. The voltage readings returned by getVoltage()
measure voltage at main power connector; they are useless if RoverWing is
powered by the  Feather board.


Written in 2019 by David Bershadsky, Alexander Kirillov

This example code is in the public domain.
*/
#include <Wire.h>
#include <RoverWing.h>

Rover r; //this is the name of the rover!
float yaw, pitch, roll;
uint8_t i;
bool blink=false;
//if this is the first time using the IMU, calibration is needed
// if you have already calibrated the IMU recentlym there is no need to repeat
// it; in this case, repalce the value below by false  
bool needCalibration = true;


void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();
  Wire.setClock(400000); //use fast mode (400 kHz)
  Serial.begin(9600); //debugging terminal
  delay(500); //wait for 0.5 second, so that roverwing initializes
  Serial.print("Connecting to RoverWing");
  r.beginVerbose();
  Serial.println("Initializing the IMU");
  r.IMUbegin();
  delay(500);
  if (! r.IMUisActive() ){
    Serial.println("IMU not found!");
    return;
  }
  //if we reached here, IMU is ok
  if (needCalibration) {
    Serial.println("Starting calibration. Please make sure robot is completely stationary and level.");
    Serial.println("The process will take about 10 seconds");
    //delay(1000);
    r.IMUcalibrate();
    Serial.println("Calibration complete.");
  }

  Serial.println("Now, let us test the   IMU...");
  delay(2000);
}
void loop(){
  Serial.print("Yaw: "); Serial. print(r.getYaw());  Serial.print(" ");
  Serial.print("Pitch: "); Serial. print(r.getPitch());  Serial.print(" ");
  Serial.print("Roll: "); Serial. print(r.getRoll());  Serial.println(" ");
  // if desired, uncomment the lines below to see raw gyro and accelerometer data:
  /*
    r.getAccel();
    Serial.print("Accel (g): ");
    Serial.print(r.ax); Serial.print(" ");
    Serial.print(r.ay); Serial.print(" ");
    Serial.print(r.az); Serial.print("     ");
    r.getGyro();
    Serial.print("Gyro (deg/s): ");
    Serial.print(r.gx); Serial.print(" ");
    Serial.print(r.gy); Serial.print(" ");
    Serial.print(r.gz); Serial.println(" ");
  */
  delay(300);
  //do the blink
  digitalWrite(LED_BUILTIN, blink);
  blink=!blink;
}
