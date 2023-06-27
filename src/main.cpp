#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <NTPClient.h>

#define RELAY_PIN 15

const char* ssid = "JARMESON_JNETCOM";
const char* password = "wet20110";

bool timerActivate = true;

int minTimerHours = 22;
int minTimerMinutes = 00;
int maxTimerHours = 06;
int maxTimerMinutes = 00;

int previousDay = 0;
int timeInSeconds = 0;

WiFiUDP ntpUDP;
WebServer server(80);
NTPClient ntp(ntpUDP);
TaskHandle_t TimerTaskHandle;

int getTimerInSeconds();
void configTimer();
void getTimer();
void setStatus();
void getStatus();
void timer(void * parameters);

void setup() {
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(2, OUTPUT);

    WiFi.begin(ssid, password);
    while(WiFi.status() != WL_CONNECTED) {
        digitalWrite(2, HIGH);
        delay(200);
        digitalWrite(2, LOW);
        delay(200);
    }

    server.enableCORS();

    server.on("/config", HTTP_POST, configTimer);
    server.on("/config", HTTP_GET, getTimer);

    server.on("/status", HTTP_GET, getStatus);
    server.on("/status", HTTP_PUT, setStatus);

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

    xTaskCreate(
        timer,
        "timer",
        4000,
        NULL,
        1,
        &TimerTaskHandle
    );

    // prepare default timer
    timeInSeconds = getTimerInSeconds();
}

void loop() {
    server.handleClient();
}

int getTimerInSeconds() {
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

    return (timeDiff * 3600) + (minutesDiff * 60);
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
    
        timeInSeconds = getTimerInSeconds();
        previousDay = 0;

        server.send(201);
    } else {
        server.send(400);
    }
}

void getTimer() {
    String start = String(minTimerHours) + ":" + String(minTimerMinutes);
    String end = String(maxTimerHours) + ":" + String(maxTimerMinutes);
    String response = "{\"start\":\"" + start + "\",\"end\":\"" + end + "\"}"; 
    server.send(200, "application/json", response);
}

void setStatus() {
    String status = server.arg("status");

    if(status) {
        if(status == "on") {
            timerActivate = true;
        } else {
            timerActivate = false;
        }

        server.send(200);
    } else {
        server.send(400);
    }
}

void getStatus() {
    String status = timerActivate ? "on" : "off";
    String response = "{\"status\":\"" + status + "\"}";
    server.send(200, "application/json", response);
}

void timer(void * parameters) {
    int hours, minutes;
    unsigned long cEpoch, waitAtEpoch;

    for(;;) {
        if(timerActivate) {
            ntp.forceUpdate();
            hours = ntp.getHours();
            minutes = ntp.getMinutes();

            if(ntp.getDay() != previousDay) {
                if(hours >= minTimerHours && minutes >= minTimerMinutes) {
                    cEpoch = ntp.getEpochTime();
                    waitAtEpoch = cEpoch + timeInSeconds;
                    previousDay = ntp.getDay();

                    digitalWrite(RELAY_PIN, HIGH);

                    while(ntp.getEpochTime() < waitAtEpoch) {
                        delay(5000);
                    }

                    digitalWrite(RELAY_PIN, LOW);
                }
            }

            delay(1000);
        }
    }
}