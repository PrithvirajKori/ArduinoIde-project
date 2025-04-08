#include <TinyGPS++.h>
#include <SoftwareSerial.h>


#define GPS_RX_PIN 33 
#define GPS_TX_PIN 32 


TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);


void setup() {
    Serial.begin(115200);  
    gpsSerial.begin(115200); 


    Serial.println("Getting GPS data...");
}


void loop() {
    // Read data from GPS module
    while (gpsSerial.available() > 0) {
        if (gps.encode(gpsSerial.read())) {
            if (gps.location.isValid()) {
                // Get latitude and longitude
                float latitude = gps.location.lat();
                float longitude = gps.location.lng();


                // Print latitude and longitude
                Serial.print("Latitude: ");
                Serial.println(latitude, 7);
                Serial.print("Longitude: ");
                Serial.println(longitude, 7);
            }
        }
    }
}