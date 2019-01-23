#include "Udp.h"

uint8_t* udpRawIPAddress(struct UDP *udp, struct IPAddress *addr)
{
  return raw_address(addr);
}
