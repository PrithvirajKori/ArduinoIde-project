#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Prithviraj";       // Mobile hotspot name
const char* password = "PrithviraJK123";   // Mobile hotspot password

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Connected to WiFi");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  server.on("/gps", HTTP_POST, [](AsyncWebServerRequest *request){
    if (request->hasParam("lat", true) && request->hasParam("lng", true)) {
      String lat = request->getParam("lat", true)->value();
      String lng = request->getParam("lng", true)->value();
      Serial.println("📍 Location Received:");
      Serial.println("Latitude: " + lat);
      Serial.println("Longitude: " + lng);
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Missing GPS data");
    }
  });

  server.begin();
}

void loop() {
  // nothing needed here
}
