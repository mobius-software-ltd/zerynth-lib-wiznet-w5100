#include "Transport.h"

uint8_t* clientRawIPAddress(struct Client *client, struct IPAddress *addr)
{
  return raw_address(addr);
}

uint8_t* udpRawIPAddress(struct UDP *udp, struct IPAddress *addr)
{
  return raw_address(addr);
}
