#include "Client.h"

uint8_t* clientRawIPAddress(struct Client *client, struct IPAddress *addr)
{
  return raw_address(addr);
}
