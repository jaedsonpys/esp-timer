#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define RPIN 15

const char* ssid = "JARMESON_JNETCOM";
const char* password = "wet20110";

WiFiUDP ntpUDP;
NTPClient ntp(ntpUDP);

void setup() {
    pinMode(RPIN, OUTPUT);
    pinMode(2, OUTPUT);

    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED) {
        digitalWrite(2, HIGH);
        delay(200);
        digitalWrite(2, LOW);
        delay(200);
    }

    ntp.begin();
    // UTC -3 (-10800)
    ntp.setTimeOffset(-10800);

    for(int i = 0; i < 3; i++) {
        digitalWrite(2, HIGH);
        delay(200);
        digitalWrite(2, LOW);
        delay(200);
    }
}

void loop() {

}