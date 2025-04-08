#include <TinyGPS++.h>
#include <HardwareSerial.h>

#define GPS_RX_PIN 33  // RX of ESP32 <-- TX of GPS
#define GPS_TX_PIN 32  // TX of ESP32 --> RX of GPS

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);  // Use UART1

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(115200, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  
  delay(2000);
  Serial.println();
  Serial.println("Bharat Pi + SkyTraq PX1125S-01A NavIC GPS Tracker");
  Serial.println("Waiting for GPS signal...");
}

void loop() {
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    gps.encode(c);

    if (gps.location.isUpdated()) {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 7);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 7);
      Serial.print("Google Maps: ");
      Serial.print("https://maps.google.com/?q=");
      Serial.print(gps.location.lat(), 7);
      Serial.print(",");
      Serial.println(gps.location.lng(), 7);
      
      Serial.print("Speed (km/h): ");
      Serial.println(gps.speed.kmph());

      Serial.print("Satellites: ");
      Serial.println(gps.satellites.value());

      Serial.print("Altitude (m): ");
      Serial.println(gps.altitude.meters());
      
      Serial.println("-------------------------");
    }
  }
}