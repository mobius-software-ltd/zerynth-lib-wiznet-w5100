#ifndef socket_h_
#define socket_h_
#include "utility/w5100.h"
#include "../cores/Transport.h"

#if defined(RAMEND) /*&& defined(RAMSTART) && ((RAMEND - RAMSTART) <= 2048)*/
#define MAX_SOCK_NUM 4
#else
#define MAX_SOCK_NUM 8
#endif

void delayMicroseconds(uint32_t);

void socketPortRand(uint16_t n);
uint8_t socketBegin(uint8_t protocol, uint16_t port);
uint8_t socketBeginMulticast(uint8_t protocol, struct IPAddress * ip, uint16_t port);
uint8_t socketStatus(uint8_t s);
void socketClose(uint8_t s);
void socketConnect(uint8_t s, uint8_t * addr, uint16_t port);
void socketDisconnect(uint8_t s);
uint8_t socketListen(uint8_t s);
uint16_t socketSend(uint8_t s, const uint8_t * buf, uint16_t len);
uint16_t socketSendAvailable(uint8_t s);
int socketRecv(uint8_t s, uint8_t * buf, int16_t len);
uint16_t socketRecvAvailable(uint8_t s);
uint8_t socketPeek(uint8_t s);
bool socketStartUDP(uint8_t s, uint8_t* addr, uint16_t port);
uint16_t socketBufferData(uint8_t s, uint16_t offset, const uint8_t* buf, uint16_t len);
bool socketSendUDP(uint8_t s);

#endif
