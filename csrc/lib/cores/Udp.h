#ifndef udp_h
#define udp_h

#include "Stream.h"
#include "IPAddress.h"

typedef struct UDP UDP;
struct UDP {
    unsigned long _timeout;      
    unsigned long _startMillis;  
    int       (*beginPacketIP)      (UDP *, struct IPAddress ip, uint16_t port);
    int       (*beginPacketHost)    (UDP *, const char *host, uint16_t port);
    int       (*endPacket)          (UDP *);
    size_t    (*writeByte)          (UDP *, uint8_t);
    size_t    (*writeBuffer)        (UDP *, const uint8_t *buffer, size_t size);
    int       (*parsePacket)        (UDP *);
    int       (*available)          (UDP *);
    int       (*read)               (UDP *);
    int       (*readBytes)          (UDP *, unsigned char* buffer, size_t len);
    int       (*readChars)          (UDP *, char* buffer, size_t len);
    int       (*peek)               (UDP *);
    void      (*flush)              (UDP *);
    struct IPAddress (*remoteIP)    (UDP *);
    uint16_t  (*remotePort)         (UDP *);
};

uint8_t* udpRawIPAddress(struct UDP *udp, struct IPAddress *addr);

#endif
