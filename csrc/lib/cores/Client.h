#ifndef client_h
#define client_h

//#include "Print.h"
#include "Stream.h"
#include "IPAddress.h"

typedef struct Client Client;
struct Client {
  int     (*connectIP)    (Client*, struct IPAddress ip, uint16_t port);
  int     (*connectHost)  (Client*, const char *host, uint16_t port);
  size_t  (*writeByte)    (Client*, uint8_t);
  size_t  (*writeBuffer)  (Client*, const uint8_t *buf, size_t size);
  int     (*available)    (Client*);
  int     (*read)         (Client*);
  int     (*readBuffer)   (Client*, uint8_t *buf, size_t size);
  int     (*peek)         (Client*);
  void    (*flush)        (Client*);
  void    (*stop)         (Client*);
  uint8_t (*connected)    (Client*);
};

uint8_t* clientRawIPAddress(struct Client *client, struct IPAddress *addr);

#endif
