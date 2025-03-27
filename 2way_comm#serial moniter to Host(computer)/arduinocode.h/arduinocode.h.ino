#include <WiFi.h>
#include <WebSocketsServer.h>

const char* ssid = "Prithviraj";       // Change to your WiFi SSID
const char* password = "PrithviraJK123"; // Change to your WiFi Password

WebSocketsServer webSocket(81); // WebSocket server on port 81

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi.");
    Serial.print("WebSocket Server IP: ");
    Serial.println(WiFi.localIP());  // Copy this IP address

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();

    // Read from Serial Monitor and send to WebSocket clients
    if (Serial.available()) {
        String message = Serial.readStringUntil('\n');
        webSocket.broadcastTXT("ESP32: " + message);
    }
}

// WebSocket Event Handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_TEXT:
            Serial.print("Received from Host: ");
            Serial.println((char*)payload); // Print message from WebSocket client
            webSocket.broadcastTXT("PC: " + String((char*)payload)); // Send to all clients
            break;
    }
}
