#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "WiFi.h"

// Wi-Fi Credentials
const char* ssid = "Prithviraj";
const char* password = "PrithviraJK123";

// AWS IoT Credentials
const char* aws_mqtt_server = "a1avjuxrgsjczi-ats.iot.ap-south-1.amazonaws.com";
const char* aws_mqtt_topic_pub = "bharatpi/pub";  // Topic to publish messages
const char* aws_mqtt_topic_sub = "bharatpi/sub";  // Topic to receive messages

// Certificate & Key (Use your own AWS IoT certificates)
const char* certificate = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWjCCAkKgAwIBAgIVAJ4/uRLdI93mmBC3ieutcXvz1Zu6MA0GCSqGSIb3DQEB
CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t
IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNTAzMTMwNjU3
NTJaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh
dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC479BI0iOetAks+GJy
LLFvDCX6wPvrcnUXOoIV0Iv6gvfzpXFqM7y+HrrKZW+H/gT1y8h0GhmiT6aGaJig
dw0sS2VvFg58NCuLmwMmPI9b//SUbAjlPtfC4LNv2487WogjJZaFImwYvSscF+cb
XW9E6ziCB2he/NgRPF+tIhp8TngM1CLJLPrms8hcRLWy0OsRL/Rh6YKnZeFnpJ45
pv/ghcLxuRTgCLJHWJnOQYge+kILYohe3aZfHw28JyHJVQIFbRetoNSekNE2XoIN
7dwL9KamyGWQDIE+IFHLOm0xINrxxNxBbRUc57owJ0kjH7lfoXVHt6lscF/towva
m7z1AgMBAAGjYDBeMB8GA1UdIwQYMBaAFF38EXIky03qoFDFW9kkn1SUtiv4MB0G
A1UdDgQWBBTDBZQd8znGHRpDCm/UWdfWwznbXTAMBgNVHRMBAf8EAjAAMA4GA1Ud
DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAIowyrWYyQDzaXzWDhHc1vQRo
UEV+XvqSSAIvDCDCmgVQQGhbJBekzq/hwbzVcMofK5zqiOuZrX9N6ooUhdAOFGIF
ojYDjy0TmfmiOrWmt/oinmFuY/hilPNNJRH7O2j99joRtAwHUPca0/MsW1b1rNnz
yuIFU3I3fc+SJcLfebPq/ODHQ5OBPnrNUc/z6W9Xf5ov8u+WFZWujM0otZdXoUBb
cQtv66+2l1f+SLrrH/CX3khZxV4zw+Y33imQvofvv/0BsSFL3e+5Mvkr2Ml12n3f
+RbI36dFQs3pYEuzS2tlOHi+aCbWEiQ4jKCI0fgYLIuqJC76h447POEnXWVTTA==
-----END CERTIFICATE-----
)EOF";

const char* private_key = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAuO/QSNIjnrQJLPhiciyxbwwl+sD763J1FzqCFdCL+oL386Vx
ajO8vh66ymVvh/4E9cvIdBoZok+mhmiYoHcNLEtlbxYOfDQri5sDJjyPW//0lGwI
5T7XwuCzb9uPO1qIIyWWhSJsGL0rHBfnG11vROs4ggdoXvzYETxfrSIafE54DNQi
ySz65rPIXES1stDrES/0YemCp2XhZ6SeOab/4IXC8bkU4AiyR1iZzkGIHvpCC2KI
Xt2mXx8NvCchyVUCBW0XraDUnpDRNl6CDe3cC/SmpshlkAyBPiBRyzptMSDa8cTc
QW0VHOe6MCdJIx+5X6F1R7epbHBf7aML2pu89QIDAQABAoIBACDZcBzPPsZ84jhb
sljneWjpNoHQ766FuSi0GtE3JvYLd3GOzJN5tyfqG3HCDMqSI11poiSFUlJTZZfw
UlQGcY+RoOXXfCExvx5gHXIYRbqov0/x1K6rt/WEpjrTPCDl1tKvZYomcB8A0vx9
fm6M7SvHwFTQbgiZTuPY3oHJUf0MQc3ZOGoARJPNqv7ZkJKXgRMW+mfM0j2IpUBG
66/M9UAIozj2Fc2wXorxfOrF/o0aBoNkPBgGJvBg7MWwxwqwXh/aQyrVt7HMSEok
HK6aNzmJ5ZuZXQKoXi6/36xHoslV3GHiwu4vwQeMgDW+vyexURr87SxAiuhFO3ET
gwAjjKECgYEA4MSguCyfnZst4LvypFnzP99cTuxe0lBxHOrBVMQsVZGCUy719ErP
u7ACtXDHmXLk+X8LHaHhHOZYGluxQfc0qwhUjISmfmDsRh3lM4FROT0Z1iZ0y/c8
mvC/g8YHsDbti6yz985m1H/GvzVDdd3wExiy9VeB18/A0IkpNtIcxD0CgYEA0qJS
BcE9lukVu15VpJ53arQJI96iBUi1U9m1hiOMMPBue/mJHpvQ6ugstJ3NeEqPsfFr
MsltmEKdoDbiifTt8KL3n01Jtq7aqW8njBvotRwx/Ahz9SsAZNC1ZVEPqSRSR45y
lN06H/5+jvQju+fpMaw+nDyztxKfKTbjm7tRDxkCgYEApS57ur44k1fukGEvga8F
QCSDLxEw4jjx18GCUSxWNJBaCY3SfTTZk2JXq0X1ugSg1+aQobdMu7NgfgZyrzrg
Pu1BTX9S6D+MhD8GKSLZMUqBCUt7vEW3CVryNAI9tje3vVUvcWcDVRmOUwjuydlP
zIjzQ7CKoLJyTg7u9fXV5DUCgYEAw518D1wnlr9hL2Gsp/WDcdVBp85bKg0cWYMW
0jyxNcIU2Y4ligerPn3hBtyGtfCJAvjyVXg8D+vN970BJp4RXCn9TjS5AK/i4vgY
AkbpF0nYIfi3uDj3ZKlvzWg64xqYT1w7jYTnNk9jud+CUEBQkZEllHPLc3oX1m7j
cv3DigECgYALYku7+uCuiukOUCb6A9/3PrhTObDxjIocCJHNGuqsj6xFa5YcrTmC
p4ySRA0Av/hAUeQukagTuZOJ8OeQ8rq2GS12TyXMHH9u5c4DKXA8wkWH26YOdr2M
QaE4NFoqjbB7tQaLL3ur56473iIHAEg8oqaYbysHxPTeE3GZyrnyGA==
-----END RSA PRIVATE KEY-----
)EOF";

const char* rootCA = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

WiFiClientSecure wifiClient;
PubSubClient client(wifiClient);

// Callback function to receive messages from AWS IoT
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message from AWS IoT: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

// Connect to AWS IoT
void connectAWS() {
    Serial.print("Connecting to AWS IoT...");
    wifiClient.setCACert(rootCA);
    wifiClient.setCertificate(certificate);
    wifiClient.setPrivateKey(private_key);

    client.setServer(aws_mqtt_server, 8883);
    client.setCallback(callback);

    while (!client.connected()) {
        Serial.print(".");
        if (client.connect("BharatPiClient")) {
            Serial.println("Connected to AWS IoT!");
            client.subscribe(aws_mqtt_topic_sub); // Subscribe to receive messages
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" Retrying in 5 seconds...");
            delay(5000);
        }
    }
}

// Connect to Wi-Fi
void connectWiFi() {
    Serial.print("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("Connected!");
}

void setup() {
    Serial.begin(115200);
    connectWiFi();
    connectAWS();
}

void loop() {
    if (!client.connected()) {
        connectAWS();
    }
    client.loop();

    // Check if there's data from the Serial Monitor
    if (Serial.available()) {
        String message = Serial.readStringUntil('\n'); // Read input from Serial Monitor
        message.trim();
        if (message.length() > 0) {
            Serial.print("Sending to AWS IoT: ");
            Serial.println(message);
            client.publish(aws_mqtt_topic_pub, message.c_str()); // Publish to AWS IoT
        }
    }
}
