/*
RoverWing Drive  test

This code is part of RoverWing library: https://github.com/roverwing

This code demonstrates higher level driving function such as "drive forward" or "turn 90 degrees "
It assumes that your rover is using "tank drive", with two identical motors,
one of which powers the left side, and the other, the right side. See the user guide for details.

It also assumes that you have the RoverWing Top attached


You need to change various pieces of configuration to match your robot;
they are indicate by CHANGE AS NEEDED in comments

It is assumed that IMU has already been calibrated - otherwise, add IMU
calibration (check sample code in "IMU" example sketch).

Written in 2020 by David Bershadsky, Alexander Kirillov

This example code is in the public domain.
*/
#include <Wire.h>
#include <RoverWing.h>
#include <RoverWingTop.h> //library for use with the top display and buttons


//pin numbers for buttons.
//Button A uses pin next to SCL; button B, the next pin
//For feather boards based on Cortex M0, Atmega 328P, or 32u4 chipsets, the pin numbers are
// 5 (button A) and 6 (button B)
//For HUZZAH32-ESP32 Feather, these are pin numbers 14 (button A) and 32 (button B)
#define BUTTON_A 5 //CHANGE AS NEEDED
#define BUTTON_B 6

Rover r; //this is the name of the rover!
motorconfig_t myMotor;    //to hold configuration data for the motors
driveconfig_t drivetrain; // to hold configuration data about robot



void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  Wire.begin();
  Wire.setClock(400000); //use fast mode (400 kHz)
  topDisplay.begin();
  topDisplay.clearBuffer();
  delay(1000); //wait for 1 second, so that roverwing initializes
  //activates RoverWing and prints basic info to Serial
  r.beginVerbose();
  String line2="FW version: "+r.fwVersion();
  String line3="Voltage: "+String(r.getVoltage());
  displayMessage("Connected", line2, line3);
  r.IMUbegin();
  delay(2000);
  while (! r.IMUisActive() ) {
    Serial.print(".");
    delay(200);
  }
  displayMessage("Press A", "to calibrate");
  bool calibrate=waitForButton(BUTTON_A, 3000); //timeout of 3 sec
  if (calibrate) {
    displayMessage("Calibrating", "the IMU...");
    delay(500);
    r.IMUcalibrate();
    displayMessage("Done");
    delay(4000);
  }
  displayMessage("Press B", "to continue");
  waitForButton(BUTTON_B);
  //configure the drive - CHANGE AS NEEDED
  myMotor.noloadRPM=180;   //motor RPM
  r.configureMotor(MOTOR1, myMotor);
  r.configureMotor(MOTOR2, myMotor);
  //now, setup the drivetrain conifuration. Read the user guide for detailed explanations
  drivetrain.leftMotor=MOTOR1;
  drivetrain.rightMotor=MOTOR2;
  drivetrain.rightMotorReversed=true;
  drivetrain.wheelDiameter=40;//in mm
  drivetrain.wheelBase=170;   //in mm
  drivetrain.minPower=0.1;   // need at least 10% to move
  //now save these values
  r.configureDrive(drivetrain);
  r.setDriveRampTime(1000); //set ramping up time to be 1 sec

}
void loop(){
  r.setTopLED(GREEN,OFF, GREEN);
  //go forward for 3 sec
  r.startForward(0.4);
  delay(3000);
  //stop and wait 1 sec
  r.stop();
  r.setTopLED(OFF,YELLOW, OFF);
  delay(1000);
  //turn 90 deg right
  r.setTopLED(BLUE,OFF, OFF);
  r.turn(0.3, 88); //turn clockwise by 90 degree at 30% power
  r.setTopLED(OFF);
  displayMessage("Yaw", String(r.getYaw()));
  delay(1000);
}
