#ifndef IPAddress_h
#define IPAddress_h

#include <stdint.h>
#include <stdbool.h>

struct IPAddress {
  union {
    uint8_t bytes[4];  // IPv4 address
    uint32_t dword;
  } _address;
};

extern struct IPAddress INADDR_NONE;

uint8_t* raw_address(struct IPAddress * ipAddress);
// Constructors
void emptyIPAddress(struct IPAddress *ipAddress);
void octetsIPAddress(struct IPAddress *ipAddress, uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet);
void dwordIPAddress(struct IPAddress *ipAddress, uint32_t address);
void intIPAddress(struct IPAddress *ipAddress, const uint8_t *address);
bool ipAddressfromString(struct IPAddress *ipAddress, const char *address);

uint32_t getIP(struct IPAddress ipAddress);
bool equalsIPAddressDword(struct IPAddress *ipAddress, uint32_t address);
bool equalsIPAddressInt(struct IPAddress *ipAddress, uint8_t* address);

uint8_t setOctetIPAddress(struct IPAddress *ipAddress, int index, uint8_t octet);
uint8_t getOctetIPAddress(struct IPAddress *ipAddress, int index);

#endif
