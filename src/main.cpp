#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <NTPClient.h>

#define RPIN 15

const char* ssid = "JARMESON_JNETCOM";
const char* password = "wet20110";

int minTimerHours = 23;
int minTimerMinutes = 13;
int maxTimerHours = 23;
int maxTimerMinutes = 15;

int previousDay = 0;
int timeInSeconds;

WiFiUDP ntpUDP;
WebServer server(80);
NTPClient ntp(ntpUDP);

void configTimer();

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
    server.begin();

    // UTC -3 (-10800): Brasília
    ntp.setTimeOffset(-10800);

    for(int i = 0; i < 3; i++) {
        digitalWrite(2, HIGH);
        delay(200);
        digitalWrite(2, LOW);
        delay(200);
    }

    // RELAY NO (Normally Open) mode
}

void loop() {
    ntp.forceUpdate();
    int hours = ntp.getHours();
    int minutes = ntp.getMinutes();

    if(ntp.getDay() != previousDay) {
        if(hours >= minTimerHours && minutes >= minTimerMinutes) {
            unsigned long cEpoch = ntp.getEpochTime();
            unsigned long waitAtEpoch = cEpoch + timeInSeconds;
            previousDay = ntp.getDay();

            digitalWrite(RPIN, HIGH);

            while(ntp.getEpochTime() < waitAtEpoch) {
                delay(5000);
            }

            digitalWrite(RPIN, LOW);
        }
    }

    delay(1000);
}

void configTimer() {
    String sH = server.arg("sh");
    String sM = server.arg("sm");
    String eH = server.arg("eh");
    String eM = server.arg("em");

    if(sH && sM && eH && eM) {
        minTimerHours = sH.toInt();
        minTimerMinutes = sM.toInt();
        maxTimerHours = eH.toInt();
        maxTimerMinutes = eM.toInt();

        int timeDiff, minutesDiff;
    
        if(minTimerHours > maxTimerHours) {
            int minHourSub = 24 - minTimerHours;
            timeDiff = maxTimerHours + minHourSub;
        } else {
            timeDiff = maxTimerHours - minTimerHours;
        }

        minutesDiff = maxTimerMinutes - minTimerMinutes;

        if(minutesDiff < 0) {
            minutesDiff *= -1;
        }

        timeInSeconds = (timeDiff * 3600) + (minutesDiff * 60);
        server.send(201);
    } else {
        server.send(400);
    }
}