//#pragma once

#ifndef	W5100_H_INCLUDED
#define	W5100_H_INCLUDED

#include "../../libraries/SPI.h"

typedef uint8_t SOCKET;

typedef enum {
  SnMR_CLOSE  = 0x00,
  SnMR_TCP    = 0x21,
  SnMR_UDP    = 0x02,
  SnMR_IPRAW  = 0x03,
  SnMR_MACRAW = 0x04,
  SnMR_PPPOE  = 0x05,
  SnMR_ND     = 0x20,
  SnMR_MULTI  = 0x80
} SnMR;

typedef enum {
  Sock_OPEN      = 0x01,
  Sock_LISTEN    = 0x02,
  Sock_CONNECT   = 0x04,
  Sock_DISCON    = 0x08,
  Sock_CLOSE     = 0x10,
  Sock_SEND      = 0x20,
  Sock_SEND_MAC  = 0x21,
  Sock_SEND_KEEP = 0x22,
  Sock_RECV      = 0x40
} SockCMD;

typedef enum {
  SnIR_SEND_OK = 0x10,
  SnIR_TIMEOUT = 0x08,
  SnIR_RECV    = 0x04,
  SnIR_DISCON  = 0x02,
  SnIR_CON     = 0x01
} SnIR;

typedef enum {
  SnSR_CLOSED      = 0x00,
  SnSR_INIT        = 0x13,
  SnSR_LISTEN      = 0x14,
  SnSR_SYNSENT     = 0x15,
  SnSR_SYNRECV     = 0x16,
  SnSR_ESTABLISHED = 0x17,
  SnSR_FIN_WAIT    = 0x18,
  SnSR_CLOSING     = 0x1A,
  SnSR_TIME_WAIT   = 0x1B,
  SnSR_CLOSE_WAIT  = 0x1C,
  SnSR_LAST_ACK    = 0x1D,
  SnSR_UDP         = 0x22,
  SnSR_IPRAW       = 0x32,
  SnSR_MACRAW      = 0x42,
  SnSR_PPPOE       = 0x5F
} SnSR;

typedef enum {
  IPPROTO_IP   = 0,
  IPPROTO_ICMP = 1,
  IPPROTO_IGMP = 2,
  IPPROTO_GGP  = 3,
  IPPROTO_TCP  = 6,
  IPPROTO_PUP  = 12,
  IPPROTO_UDP  = 17,
  IPPROTO_IDP  = 22,
  IPPROTO_ND   = 77,
  IPPROTO_RAW  = 255
} IPPROTO;

typedef enum {
  UNKNOWN,
  LINK_ON,
  LINK_OFF
} W5100Linkstatus;

typedef struct W5100Class{
  uint8_t chip;
  uint8_t CH_BASE_MSB; // 1 redundant byte, saves ~80 bytes code on AVR
  uint8_t ss_pin;
  uint32_t *ss_pin_reg;
	uint32_t ss_pin_mask;
  uint16_t SSIZE;
  uint16_t SMASK;
} W5100Class;

extern W5100Class *w5100Clazz;

uint8_t w5100ClassInit(W5100Class *w5100);
void w5100ClassSetGatewayIp(W5100Class *w5100, const uint8_t * addr);
void w5100ClassGetGatewayIp(W5100Class *w5100, uint8_t * addr);
void w5100ClassSetSubnetMask(W5100Class *w5100, const uint8_t * addr);
void w5100ClassGetSubnetMask(W5100Class *w5100, uint8_t * addr);
void w5100ClassSetMACAddress(W5100Class *w5100, const uint8_t * addr);
void w5100ClassGetMACAddress(W5100Class *w5100, uint8_t * addr);
void w5100ClassSetIPAddress(W5100Class *w5100, const uint8_t * addr);
void w5100ClassGetIPAddress(W5100Class *w5100, uint8_t * addr);
void w5100ClassSetRetransmissionTime(W5100Class *w5100, uint16_t timeout);
void w5100ClassSetRetransmissionCount(W5100Class *w5100, uint8_t retry);
void w5100ClassExecCmdSn(W5100Class *w5100, SOCKET s, SockCMD _cmd);
uint16_t writeBuffer(W5100Class *w5100, uint16_t addr, const uint8_t *buf, uint16_t len);
uint8_t writeByte(W5100Class *w5100, uint16_t addr, uint8_t data);
uint16_t readBuffer(W5100Class *w5100, uint16_t addr, uint8_t *buf, uint16_t len);
uint8_t readByte(W5100Class *w5100, uint16_t addr);
W5100Linkstatus getLinkStatus(W5100Class *w5100);
void writeMR(W5100Class *w5100, uint8_t _data);
uint8_t readMR(W5100Class *w5100);
void writeIR(W5100Class *w5100, uint8_t _data);
uint8_t readIR(W5100Class *w5100);
void writeIMR(W5100Class *w5100, uint8_t _data);
uint8_t readIMR(W5100Class *w5100);
void writeRCR(W5100Class *w5100, uint8_t _data);
uint8_t readRCR(W5100Class *w5100);
void writeRMSR(W5100Class *w5100, uint8_t _data);
uint8_t readRMSR(W5100Class *w5100);
void writeTMSR(W5100Class *w5100, uint8_t _data);
uint8_t readTMSR(W5100Class *w5100);
void writePATR(W5100Class *w5100, uint8_t _data);
uint8_t readPATR(W5100Class *w5100);
void writePTIMER(W5100Class *w5100, uint8_t _data);
uint8_t readPTIMER(W5100Class *w5100);
void writePMAGIC(W5100Class *w5100, uint8_t _data);
uint8_t readPMAGIC(W5100Class *w5100);
void writeRTR(W5100Class *w5100, uint16_t _data);
uint16_t readRTR(W5100Class *w5100);
void writeUPORT(W5100Class *w5100, uint16_t _data);
uint16_t readUPORT(W5100Class *w5100);
void writeGAR(W5100Class *w5100, const uint8_t * _buff);
void readGAR(W5100Class *w5100, uint8_t *_buff);
void writeSUBR(W5100Class *w5100, const uint8_t *_buff);
void readSUBR(W5100Class *w5100, uint8_t *_buff);
void writeSHAR(W5100Class *w5100, const uint8_t *_buff);
void readSHAR(W5100Class *w5100, uint8_t *_buff);
void writeSIPR(W5100Class *w5100, const uint8_t *_buff);
void readSIPR(W5100Class *w5100, uint8_t *_buff);
void writeUIPR(W5100Class *w5100, const uint8_t *_buff);
void readUIPR(W5100Class *w5100, uint8_t *_buff);
uint16_t getCH_BASE(W5100Class *w5100);
#define CH_SIZE 0x0100
uint8_t readSn(W5100Class *w5100, SOCKET s, uint16_t addr);
uint8_t writeSn(W5100Class *w5100, SOCKET s, uint16_t addr, uint8_t data);
uint16_t readSnBuffer(W5100Class *w5100, SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len);
uint16_t writeSnBuffer(W5100Class *w5100, SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len);
void writeSnMR(W5100Class *w5100, SOCKET _s, uint8_t _data);
uint8_t readSnMR(W5100Class *w5100, SOCKET _s);
void writeSnCR(W5100Class *w5100, SOCKET _s, uint8_t _data);
uint8_t readSnCR(W5100Class *w5100, SOCKET _s);
void writeSnIR(W5100Class *w5100, SOCKET _s, uint8_t _data);
uint8_t readSnIR(W5100Class *w5100, SOCKET _s);
void writeSnSR(W5100Class *w5100, SOCKET _s, uint8_t _data);
uint8_t readSnSR(W5100Class *w5100, SOCKET _s);
void writeSnPROTO(W5100Class *w5100, SOCKET _s, uint8_t _data);
uint8_t readSnPROTO(W5100Class *w5100, SOCKET _s);
void writeSnTOS(W5100Class *w5100, SOCKET _s, uint8_t _data);
uint8_t readSnTOS(W5100Class *w5100, SOCKET _s);
void writeSnTTL(W5100Class *w5100, SOCKET _s, uint8_t _data);
uint8_t readSnTTL(W5100Class *w5100, SOCKET _s);
void writeSnPORT(W5100Class *w5100, SOCKET _s, uint16_t _data);
uint16_t readSnPORT(W5100Class *w5100, SOCKET _s);
void writeSnDPORT(W5100Class *w5100, SOCKET _s, uint16_t _data);
uint16_t readSnDPORT(W5100Class *w5100, SOCKET _s);
void writeSnMSSR(W5100Class *w5100, SOCKET _s, uint16_t _data);
uint16_t readSnMSSR(W5100Class *w5100, SOCKET _s);
void writeSnTX_FSR(W5100Class *w5100, SOCKET _s, uint16_t _data);
uint16_t readSnTX_FSR(W5100Class *w5100, SOCKET _s);
void writeSnTX_RD(W5100Class *w5100, SOCKET _s, uint16_t _data);
uint16_t readSnTX_RD(W5100Class *w5100, SOCKET _s);
void writeSnTX_WR(W5100Class *w5100, SOCKET _s, uint16_t _data);
uint16_t readSnTX_WR(W5100Class *w5100, SOCKET _s);
void writeSnRX_RSR(W5100Class *w5100, SOCKET _s, uint16_t _data);
uint16_t readSnRX_RSR(W5100Class *w5100, SOCKET _s);
void writeSnRX_RD(W5100Class *w5100, SOCKET _s, uint16_t _data);
uint16_t readSnRX_RD(W5100Class *w5100, SOCKET _s);
void writeSnRX_WR(W5100Class *w5100,SOCKET _s, uint16_t _data);
uint16_t readSnRX_WR(W5100Class *w5100,SOCKET _s);
uint16_t writeSnDHAR(W5100Class *w5100,SOCKET _s, uint8_t *_buff);
uint16_t readSnDHAR(W5100Class *w5100,SOCKET _s, uint8_t *_buff);
uint16_t writeSnDIPR(W5100Class *w5100,SOCKET _s, uint8_t *_buff);
uint16_t readSnDIPR(W5100Class *w5100,SOCKET _s, uint8_t *_buff);
uint8_t softReset(W5100Class *w5100);
uint8_t isW5100(W5100Class *w5100);
uint8_t getChip(W5100Class *w5100);
uint16_t SBASE(W5100Class *w5100, uint8_t socknum);
uint16_t RBASE(W5100Class *w5100, uint8_t socknum);

extern PinDescription _g_APinDescription[];
void pinMode( uint32_t ulPin, uint32_t ulMode );
bool hasOffsetAddressMapping(void);
void initSS(W5100Class *w5100);
void setSS(W5100Class *w5100);
void resetSS(W5100Class *w5100);

#endif

#ifndef UTIL_H
#define UTIL_H

#define htons(x) ( (((x)<<8)&0xFF00) | (((x)>>8)&0xFF) )
#define ntohs(x) htons(x)

#define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                   ((x)<< 8 & 0x00FF0000UL) | \
                   ((x)>> 8 & 0x0000FF00UL) | \
                   ((x)>>24 & 0x000000FFUL) )
#define ntohl(x) htonl(x)

#endif
