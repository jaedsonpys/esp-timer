#ifndef UDP_h
#define UDP_h
#endif

#include <Arduino.h>

class SocketUDP {
    public:
        SocketUDP(int port);

        void sendto(char* host, int port, String data);
        String listen();
};