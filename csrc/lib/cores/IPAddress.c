#include "IPAddress.h"

//struct IPAddress INADDR_NONE = {0,0,0,0}

uint8_t* raw_address(struct IPAddress * ipAddress)
{
  return ipAddress->_address.bytes;
}

emptyIPAddress(struct IPAddress *ipAddress)
{
    ipAddress->_address.dword = 0;
}

octetsIPAddress(struct IPAddress *ipAddress, uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet)
{
    ipAddress->_address.bytes[0] = first_octet;
    ipAddress->_address.bytes[1] = second_octet;
    ipAddress->_address.bytes[2] = third_octet;
    ipAddress->_address.bytes[3] = fourth_octet;
}

dwordIPAddress(struct IPAddress *ipAddress, uint32_t address)
{
    ipAddress->_address.dword = address;
}

intIPAddress(struct IPAddress *ipAddress, const uint8_t *address)
{
    memcpy(ipAddress->_address.bytes, address, sizeof(ipAddress->_address.bytes));
}

bool ipAddressfromString(struct IPAddress *ipAddress, const char *address)
{
    uint16_t acc = 0; // Accumulator
    uint8_t dots = 0;

    while (*address)
    {
        char c = *address++;
        if (c >= '0' && c <= '9')
        {
            acc = acc * 10 + (c - '0');
            if (acc > 255) {
                // Value out of [0..255] range
                return false;
            }
        }
        else if (c == '.')
        {
            if (dots == 3) {
                // Too much dots (there must be 3 dots)
                return false;
            }
            ipAddress->_address.bytes[dots++] = acc;
            acc = 0;
        }
        else
        {
            // Invalid char
            return false;
        }
    }

    if (dots != 3) {
        // Too few dots (there must be 3 dots)
        return false;
    }
    ipAddress->_address.bytes[3] = acc;
    return true;
}

uint32_t getIP(struct IPAddress ipAddress){ return ipAddress._address.dword; }

uint8_t setOctetIPAddress(struct IPAddress *ipAddress, int index, uint8_t octet)
{
    ipAddress->_address.bytes[index] = octet;
}

uint8_t getOctetIPAddress(struct IPAddress *ipAddress, int index)
{
  return ipAddress->_address.bytes[index];
}

bool equalsIPAddressDword(struct IPAddress *ipAddress, uint32_t address)
{
  return ipAddress->_address.dword == address;
}

bool equalsIPAddressInt(struct IPAddress *ipAddress, uint8_t* address)
{
  return ipAddress->_address.bytes == address;
}
