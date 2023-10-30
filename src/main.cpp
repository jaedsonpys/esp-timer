#include <Arduino.h>
#include <WiFi.h>

#include "device.h"
#include "udp.h"

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

SocketUDP socket;
Device device01("LampadaSala", 27);

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
    socket.begin(2808);
}

void loop() {
    String command, device, status;
    String timer, startTimer, endTimer;
    String startHour, startMinute;
    String endHour, endMinute;

    int sepIndex;

    command = socket.listen();

    if(command.startsWith("control")) {
        command.replace("control:", "");
        sepIndex = command.indexOf(':');

        device = command.substring(0, sepIndex);
        status = command.substring(sepIndex + 1, command.length());

        if(device == "LampadaSala") {
            if(status == "on") {
                device01.powerOn();
            } else {
                device01.powerOff();
            }
        }
    } else if(command.startsWith("timer")) {
        command.replace("timer:", "");
        sepIndex = command.indexOf(':');

        device = command.substring(0, sepIndex);
        timer = command.substring(sepIndex + 1, command.length());

        startTimer = timer.substring(0, timer.indexOf('/'));
        endTimer = timer.substring(timer.indexOf('/') + 1, timer.length());

        startHour = startTimer.substring(0, startTimer.indexOf('.'));
        startMinute = startTimer.substring(startTimer.indexOf('.') + 1, startTimer.length());
        endHour = endTimer.substring(0, endTimer.indexOf('.'));
        endMinute = endTimer.substring(endTimer.indexOf('.') + 1, endTimer.length());

        if(device == "LampadaSala") {
            device01.setTimer(
                startTimer.toInt(),
                startMinute.toInt(),
                endHour.toInt(),
                endMinute.toInt()
            );
        }
    }

    delay(100);
}