#ifndef UDP_h
#define UDP_h
#endif

#include <Arduino.h>

class SocketUDP {
    public:
        void begin(int port);
        void sendto(char* host, int port, String data);
        String listen();
};