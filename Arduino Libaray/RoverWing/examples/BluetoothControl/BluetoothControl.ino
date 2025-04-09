/*
RoverWing: a bluetooth (BLE) controlled robot 

This code is part of RoverWing library: https://github.com/roverwing

This example assumes the following setup: 
 - a mobile robot, with tank drive (one motor driving the  left side, the other, right)
 - RoverWing (of course!)
 - two sonars (they are optional)
 - A BLE(Bluetooth Low Energy) enabled Feather. This example is written for NRF52840 Feather Express; it may require modifications for other feathers

This example is based on Blynk (blynk.cc) app. You need to:
 - Install blynk app on your cellphone (it is available for both Android and iOS)
 - Install blynk library for your arduino board (available using library manager)
 - create in blynk app on your cellphone a project which would consist of the following widgets: 
   -- a BLE widget (for connecting to the Feather)
   -- two "Value Display" widgets (to dispaly sonar values). Each must be configured with the folowing settings: 
      pin: virtual pin V1 (respectively, V2)
      range: 0..1023 (doesn't matter)
      reading rate: push
   -- Joystick widget; make it as large as possible. Configure it as follows: 
      pin: virtual pin V0 
      output:Merge 
      range (for both x,y): -100..100
      autoreturn: on 
      rotate on tilt: on 
      write interval: 100ms 

Once you create the project, you will receive an email with the authorization token for that project. 
Enter this token in the sketch below, in line char auth[]=...    
 


Written in 2020 by David Bershadsky, Alexander Kirillov

This example code is in the public domain.
*/
#include <Wire.h>
#include <RoverWing.h>
#include <bluefruit.h>
#include <BlynkSimpleSerialBLE.h>
#define BLYNK_USE_DIRECT_CONNECT
#define BLYNK_PRINT Serial
char auth[] = "";//IMPORTANT: enter the authorization token for your project here!

Rover r; //this is the name of the rover!
BLEUart bleuart; // uart over ble


void setup(){
  Wire.begin();
  Wire.setClock(400000); //use fast mode (400 kHz)
  Serial.begin(9600); //debugging terminal
  delay(1000); //wait for 1 second, so that roverwing initializes
  //activates RoverWing and prints basic info to Serial
  r.beginVerbose();
  //activate sonars
  //second argument is max distance in mm
  r.activateSonars(SONAR1+SONAR2, 3000);

  r.reverseMotor(MOTOR2);
  
  //now, do the bluetooth stuff 
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin();
  Bluefruit.setTxPower(8);    // For nRF52832, maximal power is 4; for nRF52840, it is 8
  Bluefruit.setName("RoverWingBLE");
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  
  // Configure and Start BLE Uart Service
  bleuart.begin();
  // Set up and start BLE advertising
  startAdv();
  Blynk.begin(bleuart, auth);
}
void loop(){

  //get sonar values 
  r.getAllSonar(); //fetches values from RoverWing and saves them in r.sonar[] array
  Blynk.virtualWrite(V1, r.sonar[0]); //push value of SONAR1 as "virtual pin V1" to Blynk app
  Blynk.virtualWrite(V2, r.sonar[1]); //same for SONAR2
  Blynk.run(); 
  delay(10);

}

BLYNK_WRITE(V0){ // runs each time we receive a new joystick input from the app
    float x = param[0].asInt()/100.0; // get x- component of joystick  
    float  y = param[1].asInt()/100.0; // y -component
    //non-linear input in x (turn):
    x=x*fabs(x);
    r.setAllMotorPwr(y+x,y-x);
}

// advertise Bluetooth device 
void startAdv(void) {
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}
// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

//Callback invoked when a connection is dropped
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}
