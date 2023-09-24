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

WiFiServer server(80);

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
    server.begin();
}

void loop() {
    WiFiClient client = server.available();

    if(client) {
        if(client.available()) {
            String command = client.readString();
            command.trim();

            if(command.equals("control")) {
                String device = client.readString();
                String status = client.readString();
                
                device.trim();
                status.trim();

                if(device.equals("LampadaSala")) {
                    if(status.equals("on")) {
                        device01.powerOn();
                    } else {
                        device01.powerOff();
                    }
                }

                client.println("OK");
            } else if(command.equals("ping")) {
                client.println("pong");
            }
        }
    }
}