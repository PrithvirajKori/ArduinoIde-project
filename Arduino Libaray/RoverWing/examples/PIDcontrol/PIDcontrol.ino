/*
RoverWing PID speed control  test

This code is part of RoverWing library: https://github.com/roverwing

It can be used with any Adafruit Feather board (or compatible) plugged into
RoverWing  board.

For this test, RoverWing must be  powered through the main power connector,
using 7-14V power source. You need to connect a motor (or two motors) to the motor
port of the RoverWing; the motors must be equipped with encoders which can operate
at 3.3V.

Finally, you need to adjust the motor confiuration data to suit  your motors


Written in 2020 by David Bershadsky, Alexander Kirillov

This example code is in the public domain.
*/
#include <Wire.h>
#include <RoverWing.h>

//some target paremeters. CHange to match your motors' configuration
#define ENCODERCPR 1120 //encoder counts per revolution
                       // this should count all observable encoder events,
                       // rise/fall of channel A and rise/fall of channel B

#define NOLOADRPM 140   //motor no-load speed, in RPM
#define TARGETSPEED 100 //target motr speed, in RPM


Rover r; //this is the name of the rover!
bool blink=false;
int loopCount=0;
//motor power
float speed=TARGETSPEED; //target motor speed in RPM
motorconfig_t myMotor; //to hold configuration data for the motor

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();
  Wire.setClock(400000); //use fast mode (400 kHz)
  Serial.begin(9600); //debugging terminal
  delay(1000); //wait for 1 second, so that roverwing initializes
  //activates RoverWing and prints basic info to Serial
  r.beginVerbose();
  Serial.println("Starting PID  speed control  test");
  //now, configure the motors.

  // This data is for AndyMark's  NeveRest 40  gearmotor.
  // Change to match your motor.
  myMotor.encoderCPR = ENCODERCPR;   // encoder counts per revolution of output shaft
  myMotor.noloadRPM = NOLOADRPM;
  //If you do not know the best PID coefficicients for your motor,
  // try using the values below and adjust as needed. If you do know the values
  // already, you can comment these lines
  float maxspeed=myMotor.encoderCPR*myMotor.noloadRPM/60.0; //max speed in encoder counts/s
  myMotor.Kp=0.8/maxspeed;  //suggested proportional  gain. If the motor is too slow to achieve desired speed, increase; if the motor starts oscillating, decrease.
  myMotor.Ti=0.5;           // time constant for integral gain, in seconds
  myMotor.Td=0.0;           // time constant for differential gain, in seconds. to disable differential term, make Td=0
  myMotor.iLim = 0.2*myMotor.Ti/myMotor.Kp;
                         // limit on integral error; this value guarantees that integral term will be at most 0.2*maxspeed
                         // to disable integral limit, make iLim negative e.g. -1.0
  //finally, configure the motors
  r.configureMotor(MOTOR1, myMotor);
  r.configureMotor(MOTOR2, myMotor);
  r.reverseMotor(MOTOR2); //this reverses both speed control and
                          //encoder and speed readings for motor 2
}
void loop(){
  if (loopCount%40==0){
    //change direction  - every 40 cycles, or 8 sec
    r.stopMotors();
    r.resetAllEncoder();
    delay(250);
    speed=-speed;
    Serial.print("Resetting encoders and setting new motor speed: ");
    Serial.println(speed);
    r.setMotorSpeed(MOTOR1, speed);
    r.setMotorSpeed(MOTOR2, speed);
  }

  //do the blink
  digitalWrite(LED_BUILTIN, blink);
  blink=!blink;
  //now, wait
  delay(200);

  //get current encoder values - position (in revolutions) and speed (RPM)
  //r.getAllPosition();
  //Serial.print("Current motor position (in revolutions): "); Serial.println(r.position[MOTOR1]);
  //Serial.print(" "); Serial.println(r.position[MOTOR2]);
  r.getAllSpeed();
  Serial.print("Current motor speed (RPM): ");
  Serial.print(r.speed[MOTOR1]); Serial.print(" "); Serial.println(r.speed[MOTOR2]);
  loopCount++;
}
