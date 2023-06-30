#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

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

WebServer server(80);
TaskHandle_t TimerTaskHandle;

Device device01("LampadaSala", 15);

void setTimer();
void sendCORSHeader();
void getTimer();
void controlDevice();
void getDeviceStatus();

void setup() {
    Serial.begin(9600);
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

    server.on("/timer", HTTP_GET, getTimer);
    server.on("/timer", HTTP_POST, setTimer);
    server.on("/timer", HTTP_OPTIONS, sendCORSHeader);

    server.on("/device", HTTP_GET, getDeviceStatus);
    server.on("/device", HTTP_POST, controlDevice);
    server.on("/device", HTTP_OPTIONS, sendCORSHeader);

    server.begin();
 
    configTime(gmtOffsetSec, daylightOffSetSec, mainNTPServer, recoveryNTPServer);
}

void loop() {
    server.handleClient();
}

void setTimer() {
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));

    String sH = server.arg("sh");
    String sM = server.arg("sm");
    String eH = server.arg("eh");
    String eM = server.arg("em");
    String status = server.arg("status");

    if(status == "off") {
        device01.deleteTimer();
        server.send(200, F("text/pain"), F("timer deleted"));
    } else if(sH != "" && sM != "" && eH != "" && eM != "") {
        device01.setTimer(sH.toInt(), sM.toInt(), eH.toInt(), eM.toInt());
        server.send(201, F("text/pain"), F("timer created"));
    } else {
        server.send(400, F("text/pain"), F("invalid data"));
    }
}

void sendCORSHeader() {
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.sendHeader(F("Access-Control-Max-Age"), F("600"));
    server.sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,OPTIONS"));
    server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
    server.send(204);
}

void getTimer() {
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));

    String start = device01.getTimerStart();
    String end = device01.getTimerEnd();
    String status = device01.timerIsActive() ? "true" : "false";

    String startKV = "\"start\": \"" + start + "\",";
    String endKV = "\"end\": \"" + end + "\",";
    String statusKV = "\"timerActive\": " + status;
    String response = "{" + startKV + endKV + statusKV + "}"; 

    server.send(200, "application/json", response);
}

void controlDevice() {
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    String status = server.arg("status");

    if(status == "on"){
        device01.powerOn();
    } else if(status == "off") {
        device01.powerOff();
    }

    if(status == "on" || status == "off") {
        server.send(200);
    } else {
        server.send(400);
    }
}

void getDeviceStatus() {
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    String status = device01.isON() ? "on" : "off";
    String response = "{\"status\":\"" + status + "\"}";
    server.send(200, "application/json", response);
}