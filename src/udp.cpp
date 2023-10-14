#include <Arduino.h>
#include <WiFiUdp.h>
#include "udp.h"

WiFiUDP udp;

SocketUDP::SocketUDP(int port) {
    udp.begin(port);
};

void SocketUDP::sendto(char* host, int port, String data) {
    udp.beginPacket(host, port);
    udp.println(data);
    udp.endPacket();
};

String SocketUDP::listen() {
    while(udp.parsePacket() == 0) {
        delay(50);
    }

    String req = "";

    while(udp.available()) {
        char ch = udp.read();
        req += ch;
    }

    return req;
}