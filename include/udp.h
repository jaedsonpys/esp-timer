#ifndef UDP_h
#define UDP_h
#endif

#include <WiFiUdp.h>

class UDP {
    public:
        UDP(int port);

        void sendto(String host, int port, String data);
        String listen();
};