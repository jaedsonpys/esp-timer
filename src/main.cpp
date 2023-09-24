#include <Arduino.h>
#include <WiFi.h>

#include "device.h"

const char* ssid = "JARMESON_JNETCOM";
const char* password = "wet20110";

// NTP config
const char* mainNTPServer = "pool.ntp.org";
const char* recoveryNTPServer = "time.google.com";
const int daylightOffSetSec = -10800;
const long gmtOffsetSec = 0;

IPAddress ip(192, 168, 0, 150);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns1(8, 8, 8, 8);
IPAddress dns2(8, 8, 4, 4);

Device device01("LampadaSala", 15);

void setTimer();
void sendCORSHeader();
void getTimer();
void controlDevice();
void getDeviceStatus();

void setup() {
    pinMode(2, OUTPUT);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.config(ip, gateway, subnet, dns1, dns2);

    while(WiFi.status() != WL_CONNECTED) {
        digitalWrite(2, HIGH);
        delay(200);
        digitalWrite(2, LOW);
        delay(200);
    }

    configTime(gmtOffsetSec, daylightOffSetSec, mainNTPServer, recoveryNTPServer);
}

void loop() {

}