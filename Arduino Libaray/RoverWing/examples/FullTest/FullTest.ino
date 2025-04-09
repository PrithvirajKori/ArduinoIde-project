/*
RoverWing - all peripherals  test

This code is part of RoverWing library: https://github.com/roverwing

It can be used with any Adafruit Feather board (or compatible) plugged into
RoverWing  board. RoverWing board can be powered through the main power connector,
or it can get power from the Feather. The voltage readings returned by getVoltage()
measure voltage at main power connector; they are useless if RoverWing is
powered by the  Feather board.



Written in 2020 by David Bershadsky, Alexander Kirillov

This example code is in the public domain.
*/
#include <Wire.h>
#include <RoverWing.h>

Rover r; //this is the name of the rover!
#define NUM_PIXELS 8    //number of connected Neopixels; edit to match your configuration. Set to 0 if no neopixels are connected
#define LOW_VOLTAGE 7.0 //voltage threshold; if voltage drops below this limit, internal neopixel turns yellow
uint8_t hues[NUM_PIXELS]; //array of hues of pixels
//set variables below to enable/disable testing of different peripherals
bool testAnalogs=true;
bool testIMU=true;
bool testGPS=true;
bool testServos=true;
bool testSonars=true;
bool testMotors=true;

// if motors are equipped with encoders, please also provide encoder Counts per Revolution
// (of output shaft of the motor). Otherwise, leave this parameter 0
uint16_t encoderCPR=1120;
//various temporary variables
float power = 0.0; //motor power
uint32_t last_print=0; //time of last debugging print, in ms
//for blinking built-in LED
bool blink = false;


//setup
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Wire.begin();
    Wire.setClock(400000); //use fast mode (400 kHz)
    Serial.begin(9600);
    delay(500); //wait for 0.5 second, so that roverwing initializes
    Serial.print("Connecting to RoverWing");
    r.beginVerbose();
    //initialize GPS
    if (testGPS) {
        r.GPSbegin();
        Serial.println("GPS started. It may take a while to get a location fix");
    }
    //initialize  IMU
    if (testIMU){
        r.IMUbegin();
        delay(100);
        if (! r.IMUisActive() ){
            Serial.println("IMU not found!");
            return;
        }
        //calibrate
        Serial.println("Starting IMU calibration. Please make sure robot is completely stationary and level.");
        Serial.println("The process will take about 10 seconds");
        delay(1000);
        r.IMUcalibrate();
        Serial.println("Calibration complete.");
    }

    if (testMotors && (encoderCPR > 0)) {
        motorconfig_t myMotor;
        myMotor.encoderCPR = encoderCPR;  //encoder counts per revolution of output shaft
        r.configureMotor(MOTOR1, myMotor);
        r.configureMotor(MOTOR2, myMotor);
    }

    //low voltage threshold: internal neopixel will turn yellow if voltage drops below threshold
    r.setLowVoltage(LOW_VOLTAGE);

    //now let us setup the neopixels
    if (NUM_PIXELS > 0) {
        r.setPixelCount(NUM_PIXELS);
        r.setPixelBrightness(64); //1/4 of full brightness - this is already quite bright
        //setup initial hues
        // note that index i starts with 1
        for (int i = 1; i <= NUM_PIXELS; i++) {
            hues[i] = (i * 255) / NUM_PIXELS; // put hues uniformly on the color wheel
        }
    }
    if (testSonars){
        r.activateSonars(SONAR1+SONAR2+SONAR3, 3000); //max distance is 3000 mm = 3m
    }
}
void loop() {
    //analog inputs
    Serial.print("Voltage: "); Serial.println(r.getVoltage());
    if (testAnalogs) {
        r.getAllAnalog(); //fetches values from RoverBoard and saves them in r.analog[] array
        Serial.print("Analog inputs:");
        for (int i=1; i<=6; i++){
            Serial.print("  ");
            Serial.print(r.analog[i]); //note that index i runs 1..6, not 0..5 !
                                   // values of analog[i] are floats, representing
                                   // the voltage; they range 0 - 3.3 volts
        }
        Serial.println(" ");
    }

    //compute new power for servos and motors, which should be changing periodically, with period of 10 sec
    int k=millis() %10000; //ranges from 0-9999
    //now, create a sawtooth function
    if ( k <= 5000) {
        power = -1.0+2.0*(float)k/5000; //lineary increase from -1.0 (when k=0) to 1.0 (when k=5000)
    } else {
        power = -1.0+2.0*(float)(10000-k)/5000; //linearly decrease from 1.0 (when k=5000) to -1.0 (when k=10000)
    }
    if (testMotors) {
        r.setAllMotorPwr(power,power);
        if (encoderCPR > 0) {
            r.getAllPosition();
            Serial.print("Encoder 1 : ");
            Serial.print(r.position[0]); //position in revolutions of motor shaft
            Serial.print(" , ");
            Serial.print("Encoder 2 : ");
            Serial.println(r.position[1]);
       }
    }
    if (testServos){
        r.setServo(SERVO1, power);
        r.setServo(SERVO2, power);
        r.setServo(SERVO3, power);
        r.setServo(SERVO4, power);
    }

    if (testIMU){
        Serial.print("Yaw : ");
        Serial.print(r.getYaw());
        Serial.print(" , ");

        Serial.print("Pitch: ");
        Serial.print(r.getPitch());
        Serial.print(" , ");

        Serial.print("Roll: ");
        Serial.println(r.getRoll());
    }

    if (testSonars){
        r.getAllSonar();
        Serial.print("Sonar 1 : ");
        Serial.print(r.sonar[0]);
        Serial.print(" , ");
        Serial.print("Sonar 2 : ");
        Serial.print(r.sonar[1]);
        Serial.print(" , ");
        Serial.print("Sonar 3 : ");
        Serial.println(r.sonar[2]);
        Serial.println();
    }

    if (testGPS) {
        r.getGPSlocation();
        Serial.print("latitude : ");
        Serial.print(r.latitude(),6);
        Serial.print(" , ");
        Serial.print("longitude : ");
        Serial.println(r.longitude(),6);
    }

    if (NUM_PIXELS > 0 ) {
        for (int i = 1; i <= NUM_PIXELS; i++) {
            //update hues
            hues[i] += 25; // this way, at every loop we are rotating by 25/255, or approximately 1/10 of the revolution of the color wheel
            r.setPixelHSV(i, hues[i], 255, 255);
        }
        r.showPixel(); //this must be called to push the new colors to the actual neopixels
    }
    //blink the built-in LED of the feather board
    digitalWrite(LED_BUILTIN, blink);
    blink = !blink;
    delay(500);
}
