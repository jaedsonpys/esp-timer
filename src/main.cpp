#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WebServer.h>
#include <NTPClient.h>

#define RELAY_PIN 15

const char* ssid = "JARMESON_JNETCOM";
const char* password = "wet20110";

bool timerActivate = true;
bool timerIsRunning = true;
bool stopTimer = false;

int minTimerHours = 22;
int minTimerMinutes = 00;
int maxTimerHours = 06;
int maxTimerMinutes = 00;

int previousDay = 0;
int timeInSeconds = 0;

IPAddress ip(192, 168, 0, 150);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiUDP ntpUDP;
WebServer server(80);
NTPClient ntp(ntpUDP);
TaskHandle_t TimerTaskHandle;

int getTimerInSeconds();

void configTimer();
void sendCORSHeader();
void getTimer();
void setStatus();
void getStatus();
void controlRelay();
void getRelayStatus();

void timer(void * parameters);

void setup() {
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(2, OUTPUT);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.config(ip, gateway, subnet);

    while(WiFi.status() != WL_CONNECTED) {
        digitalWrite(2, HIGH);
        delay(200);
        digitalWrite(2, LOW);
        delay(200);
    }

    server.on("/config", HTTP_POST, configTimer);
    server.on("/config", HTTP_GET, getTimer);
    server.on("/config", HTTP_OPTIONS, sendCORSHeader);

    server.on("/status", HTTP_GET, getStatus);
    server.on("/status", HTTP_POST, setStatus);
    server.on("/status", HTTP_OPTIONS, sendCORSHeader);

    server.on("/device", HTTP_GET, getRelayStatus);
    server.on("/device", HTTP_POST, controlRelay);
    server.on("/device", HTTP_OPTIONS, sendCORSHeader);

    ntp.begin();
    server.begin();

    // RELAY NO (Normally Open) mode
    // UTC -3 (-10800): Brasília
    ntp.setTimeOffset(-10800);

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
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
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

void sendCORSHeader() {
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.sendHeader(F("Access-Control-Max-Age"), F("600"));
    server.sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,OPTIONS"));
    server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
    server.send(204);
}

void getTimer() {
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    String start = String(minTimerHours) + ":" + String(minTimerMinutes);
    String end = String(maxTimerHours) + ":" + String(maxTimerMinutes);
    String response = "{\"start\":\"" + start + "\",\"end\":\"" + end + "\"}"; 
    server.send(200, "application/json", response);
}

void setStatus() {
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
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
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    String status = timerActivate ? "on" : "off";
    String response = "{\"status\":\"" + status + "\"}";
    server.send(200, "application/json", response);
}

void controlRelay() {
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    String status = server.arg("status");

    if(status == "on"){
        digitalWrite(RELAY_PIN, HIGH);
    } else if(status == "off") {
        digitalWrite(RELAY_PIN, LOW);
    }

    if(status == "on" || status == "off") {
        if(timerIsRunning) {
            stopTimer = true;
        }

        server.send(200);
    } else {
        server.send(400);
    }
}

void getRelayStatus() {
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    String status = digitalRead(RELAY_PIN) ? "on" : "off";
    String response = "{\"status\":\"" + status + "\"}";
    server.send(200, "application/json", response);
}

void timer(void * parameters) {
    int hours, minutes, seconds;
    unsigned long cEpoch, waitAtEpoch;

    for(;;) {
        if(timerActivate) {
            ntp.forceUpdate();
            hours = ntp.getHours();
            minutes = ntp.getMinutes();
            seconds = ntp.getSeconds();

            if(ntp.getDay() != previousDay) {
                if(hours >= minTimerHours && minutes >= minTimerMinutes) {
                    timerIsRunning = true;

                    cEpoch = ntp.getEpochTime() - seconds;
                    waitAtEpoch = cEpoch + timeInSeconds;
                    previousDay = ntp.getDay();

                    digitalWrite(RELAY_PIN, HIGH);

                    while(true) {
                        if(ntp.getEpochTime() >= waitAtEpoch) {
                            digitalWrite(RELAY_PIN, LOW);
                            break;
                        }

                        if(stopTimer) {
                            stopTimer = false;
                            break;
                        }

                        delay(1000);
                    }

                    timerIsRunning = false;
                }
            }

            delay(1000);
        }
    }
}