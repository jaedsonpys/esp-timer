#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define RPIN 15

const char* ssid = "JARMESON_JNETCOM";
const char* password = "wet20110";

const int minTimerHours = 23;
const int minTimerMinutes = 13;
const int maxTimerHours = 23;
const int maxTimerMinutes = 15;

int previousDay = 0;
int timeInSeconds;

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

    // RELAY NO (Normally Open) mode
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