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

Device device01("LampadaSala", 14);

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
        while(client.connected()) {
            if(client.available()) {
                String command = client.readString();
                command.trim();

                if(command.startsWith("control")) {
                    command.replace("control:", "");
                    int sepIndex = command.indexOf(':');

                    String device = command.substring(0, sepIndex);
                    String status = command.substring(sepIndex + 1, command.length());

                    if(device == "LampadaSala") {
                        if(status == "on") {
                            device01.powerOn();
                        } else {
                            device01.powerOff();
                        }
                    }
                } else if(command.equals("ping")) {
                    client.println("pong");
                }
            }
        }

    }
}