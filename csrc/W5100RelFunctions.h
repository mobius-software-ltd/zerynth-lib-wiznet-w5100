#ifndef __W5100RELFUNCTIONS_H_
#define __W5100RELFUNCTIONS_H_

#include "ioLibrary_Driver/Ethernet/wizchip_conf.h"

#define W5100Address  ((uint32_t)0x60000000)

uint8_t spiReadByte(void);
void spiWriteByte(uint8_t byte);

void csEnable(void);
void csDisable(void);
void resetAssert(void);
void resetDeassert(void);

void W5100Reset(void);
#endif
