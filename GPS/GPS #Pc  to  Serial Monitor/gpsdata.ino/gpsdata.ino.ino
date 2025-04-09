#include <TinyGPS++.h>
#include <HardwareSerial.h>

TinyGPSPlus gps;
HardwareSerial GPS_Serial(2); // UART2

void setup() {
  Serial.begin(115200);
  GPS_Serial.begin(9600, SERIAL_8N1, 32, 33); // RX=32, TX=33
  Serial.println("Waiting for GPS fix...");
}

void loop() {
  while (GPS_Serial.available()) {
    gps.encode(GPS_Serial.read());
    if (gps.location.isUpdated()) {
      Serial.print("Latitude: "); Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: "); Serial.println(gps.location.lng(), 6);
      Serial.print("Speed (km/h): "); Serial.println(gps.speed.kmph());
      Serial.print("Satellites: "); Serial.println(gps.satellites.value());
      Serial.print("Altitude: "); Serial.println(gps.altitude.meters());
      Serial.println("-----");
    }
  }
}
