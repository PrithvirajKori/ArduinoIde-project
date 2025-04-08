#include <TinyGPS++.h>

#define GPS_RX_PIN 33
#define GPS_TX_PIN 32

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);  // Use Serial1

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);  // Use 9600 or check your module's baud rate
  Serial.println("Getting GPS data...");
}

void loop() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      if (gps.location.isValid()) {
        float latitude = gps.location.lat();
        float longitude = gps.location.lng();

        Serial.print("Latitude: ");
        Serial.println(latitude, 7);
        Serial.print("Longitude: ");
        Serial.println(longitude, 7);
      }
    }
  }
}
