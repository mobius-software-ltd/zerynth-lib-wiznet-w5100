#ifndef	W5100_H_INCLUDED
#define	W5100_H_INCLUDED

#include "component_pio.h"
#include "../../libraries/SPI.h"
#include "../../libraries/sam3x8e.h"

// Require Ethernet.h, because we need MAX_SOCK_NUM
#ifndef ethernet_h_
exit(EXIT_FAILURE);
#endif

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

typedef struct _W5100Class{
  uint8_t chip;
  uint8_t CH_BASE_MSB; // 1 redundant byte, saves ~80 bytes code on AVR
  uint8_t ss_pin;
  uint32_t *ss_pin_reg;
	uint32_t ss_pin_mask;
  uint16_t SSIZE;
  uint16_t SMASK;
} W5100Class;

uint8_t w5100ClassInit(W5100Class *w5100Class);
inline void w5100ClassSetGatewayIp(W5100Class *w5100Class, const uint8_t * addr);
inline void w5100ClassGetGatewayIp(W5100Class *w5100Class, uint8_t * addr);
inline void w5100ClassSetSubnetMask(W5100Class *w5100Class, const uint8_t * addr);
inline void w5100ClassGetSubnetMask(W5100Class *w5100Class, uint8_t * addr);
inline void w5100ClassSetMACAddress(W5100Class *w5100Class, const uint8_t * addr);
inline void w5100ClassGetMACAddress(W5100Class *w5100Class, uint8_t * addr);
inline void w5100ClassSetIPAddress(W5100Class *w5100Class, const uint8_t * addr);
inline void w5100ClassGetIPAddress(W5100Class *w5100Class, uint8_t * addr);
inline void w5100ClassSetRetransmissionTime(W5100Class *w5100Class, uint16_t timeout);
inline void w5100ClassSetRetransmissionCount(W5100Class *w5100Class, uint8_t retry);
void w5100ClassExecCmdSn(W5100Class *w5100Class, SOCKET s, SockCMD _cmd);

// W5100 Registers
// ---------------
uint16_t writeBuffer(W5100Class *w5100Class, uint16_t addr, const uint8_t *buf, uint16_t len);
uint8_t writeByte(W5100Class *w5100Class, uint16_t addr, uint8_t data);
uint16_t readBuffer(W5100Class *w5100Class, uint16_t addr, uint8_t *buf, uint16_t len);
uint8_t readByte(W5100Class *w5100Class, uint16_t addr);
W5100Linkstatus getLinkStatus(W5100Class *w5100Class);

// Mode
void writeMR(W5100Class *w5100Class, uint8_t _data);
uint8_t readMR(W5100Class *w5100Class);
// Interrupt
void writeIR(W5100Class *w5100Class, uint8_t _data);
uint8_t readIR(W5100Class *w5100Class);
// Interrupt Mask
void writeIMR(W5100Class *w5100Class, uint8_t _data);
uint8_t readIMR(W5100Class *w5100Class);
// Retry count
void writeRCR(W5100Class *w5100Class, uint8_t _data);
uint8_t readRCR(W5100Class *w5100Class);
// Receive memory size (W5100 only)
void writeRMSR(W5100Class *w5100Class, uint8_t _data);
uint8_t readRMSR(W5100Class *w5100Class);
// Transmit memory size (W5100 only)
void writeTMSR(W5100Class *w5100Class, uint8_t _data);
uint8_t readTMSR(W5100Class *w5100Class);
// Authentication type address in PPPoE mode
void writePATR(W5100Class *w5100Class, uint8_t _data);
uint8_t readPATR(W5100Class *w5100Class);
// PPP LCP Request Timer
void writePTIMER(W5100Class *w5100Class, uint8_t _data);
uint8_t readPTIMER(W5100Class *w5100Class);
// PPP LCP Magic Number
void writePMAGIC(W5100Class *w5100Class, uint8_t _data);
uint8_t readPMAGIC(W5100Class *w5100Class);
// Timeout address
void writeRTR(W5100Class *w5100Class, uint16_t _data);
uint16_t readRTR(W5100Class *w5100Class);
// Unreachable Port address in UDP mode (W5100 only)
void writeUPORT(W5100Class *w5100Class, uint16_t _data);
uint16_t readUPORT(W5100Class *w5100Class);
// Gateway IP address
void writeGAR(W5100Class *w5100Class, const uint8_t * _buff);
void readGAR(W5100Class *w5100Class, uint8_t *_buff);
// Subnet mask address
void writeSUBR(W5100Class *w5100Class, const uint8_t *_buff);
void readSUBR(W5100Class *w5100Class, uint8_t *_buff);
// Source MAC address
void writeSHAR(W5100Class *w5100Class, const uint8_t *_buff;
void readSHAR(W5100Class *w5100Class, uint8_t *_buff);
// Source IP address
void writeSIPR(W5100Class *w5100Class, const uint8_t *_buff);
void readSIPR(W5100Class *w5100Class, uint8_t *_buff);
// Unreachable IP address in UDP mode (W5100 only)
void writeUIPR(W5100Class *w5100Class, const uint8_t *_buff);
void readUIPR(W5100Class *w5100Class, uint8_t *_buff);

// W5100 Socket registers
// ----------------------
uint16_t getCH_BASE(W5100Class *w5100Class);
#define CH_SIZE 0x0100

uint8_t readSn(W5100Class *w5100Class, SOCKET s, uint16_t addr);
uint8_t writeSn(W5100Class *w5100Class, SOCKET s, uint16_t addr, uint8_t data);
uint16_t readSnBuffer(W5100Class *w5100Class, SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len);
uint16_t writeSnBuffer(W5100Class *w5100Class, SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len);

// Mode
void writeSnMR(W5100Class *w5100Class, SOCKET _s, uint8_t _data);
uint8_t readSnMR(W5100Class *w5100Class, SOCKET _s);
// Command
void writeSnCR(W5100Class *w5100Class, SOCKET _s, uint8_t _data);
uint8_t readSnCR(W5100Class *w5100Class, SOCKET _s);
// Interrupt
void writeSnIR(W5100Class *w5100Class, SOCKET _s, uint8_t _data);
uint8_t readSnIR(W5100Class *w5100Class, SOCKET _s);
// Status
void writeSnSR(W5100Class *w5100Class, SOCKET _s, uint8_t _data);
uint8_t readSnSR(W5100Class *w5100Class, SOCKET _s);
// Protocol in IP RAW Mode
void writeSnPROTO(W5100Class *w5100Class, SOCKET _s, uint8_t _data);
uint8_t readSnPROTO(W5100Class *w5100Class, SOCKET _s);
// IP TOS
void writeSnTOS(W5100Class *w5100Class, SOCKET _s, uint8_t _data);
uint8_t readSnTOS(W5100Class *w5100Class, SOCKET _s);
// IP TTL
void writeSnTTL(W5100Class *w5100Class, SOCKET _s, uint8_t _data);
uint8_t readSnTTL(W5100Class *w5100Class, SOCKET _s);
// Source Port
void writeSnPORT(W5100Class *w5100Class, SOCKET _s, uint16_t _data);
uint16_t readSnPORT(W5100Class *w5100Class, SOCKET _s);
// Destination Port
void writeSnDPORT(W5100Class *w5100Class, SOCKET _s, uint16_t _data);
uint16_t readSnDPORT(W5100Class *w5100Class, SOCKET _s);
// Max Segment Size
void writeSnMSSR(W5100Class *w5100Class, SOCKET _s, uint16_t _data);
uint16_t readSnMSSR(W5100Class *w5100Class, SOCKET _s);
// TX Free Size
void writeSnTX_FSR(W5100Class *w5100Class, SOCKET _s, uint16_t _data);
uint16_t readSnTX_FSR(W5100Class *w5100Class, SOCKET _s);
// TX Read Pointer
void writeSnTX_RD(W5100Class *w5100Class, SOCKET _s, uint16_t _data);
uint16_t readSnTX_RD(W5100Class *w5100Class, SOCKET _s);
// TX Write Pointer
void writeSnTX_WR(W5100Class *w5100Class, SOCKET _s, uint16_t _data);
uint16_t readSnTX_WR(W5100Class *w5100Class, SOCKET _s);
// RX Free Size
void writeSnRX_RSR(W5100Class *w5100Class, SOCKET _s, uint16_t _data);
uint16_t readSnRX_RSR(W5100Class *w5100Class, SOCKET _s);
// RX Read Pointer
void writeSnRX_RD(W5100Class *w5100Class, SOCKET _s, uint16_t _data);
uint16_t readSnRX_RD(W5100Class *w5100Class, SOCKET _s);
// RX Write Pointer (supported?)
void writeSnRX_WR(W5100Class *w5100Class,SOCKET _s, uint16_t _data);
uint16_t readSnRX_WR(W5100Class *w5100Class,SOCKET _s);
// Destination Hardw Addr
uint16_t writeSnDHAR(W5100Class *w5100Class,SOCKET _s, uint8_t *_buff);
uint16_t readSnDHAR(W5100Class *w5100Class,SOCKET _s, uint8_t *_buff);
// Destination IP Addr
uint16_t writeSnDIPR(W5100Class *w5100Class,SOCKET _s, uint8_t *_buff);
uint16_t readSnDIPR(W5100Class *w5100Class,SOCKET _s, uint8_t *_buff);

uint8_t softReset(W5100Class *w5100Class);
uint8_t isW5100(W5100Class *w5100Class);
uint8_t getChip(W5100Class *w5100Class);
uint16_t SBASE(W5100Class *w5100Class, uint8_t socknum);
uint16_t RBASE(W5100Class *w5100Class, uint8_t socknum);

/* Definitions and types for pins */
typedef enum _EAnalogChannel
{
  NO_ADC=-1,
  ADC0=0,
  ADC1,
  ADC2,
  ADC3,
  ADC4,
  ADC5,
  ADC6,
  ADC7,
  ADC8,
  ADC9,
  ADC10,
  ADC11,
  ADC12,
  ADC13,
  ADC14,
  ADC15,
  DA0,
  DA1
} EAnalogChannel;

#define ADC_CHANNEL_NUMBER_NONE 0xffffffff

// Definitions for PWM channels
typedef enum _EPWMChannel
{
  NOT_ON_PWM=-1,
  PWM_CH0=0,
  PWM_CH1,
  PWM_CH2,
  PWM_CH3,
  PWM_CH4,
  PWM_CH5,
  PWM_CH6,
  PWM_CH7
} EPWMChannel;

// Definitions for TC channels
typedef enum _ETCChannel
{
  NOT_ON_TIMER=-1,
  TC0_CHA0=0,
  TC0_CHB0,
  TC0_CHA1,
  TC0_CHB1,
  TC0_CHA2,
  TC0_CHB2,
  TC1_CHA3,
  TC1_CHB3,
  TC1_CHA4,
  TC1_CHB4,
  TC1_CHA5,
  TC1_CHB5,
  TC2_CHA6,
  TC2_CHB6,
  TC2_CHA7,
  TC2_CHB7,
  TC2_CHA8,
  TC2_CHB8
} ETCChannel;

typedef enum _EPioType
{
  PIO_NOT_A_PIN, /* Not under control of a peripheral. */
  PIO_PERIPH_A, /* The pin is controlled by the associated signal of peripheral A. */
  PIO_PERIPH_B, /* The pin is controlled by the associated signal of peripheral B. */
  PIO_PERIPH_C, /* The pin is controlled by the associated signal of peripheral C. */
  PIO_PERIPH_D, /* The pin is controlled by the associated signal of peripheral D. */
  PIO_INPUT, /* The pin is an input. */
  PIO_OUTPUT_0, /* The pin is an output and has a default level of 0. */
  PIO_OUTPUT_1 /* The pin is an output and has a default level of 1. */
} EPioType ;

typedef struct _PinDescription
{
  Pio* pPort ;
  uint32_t ulPin ;
  uint32_t ulPeripheralId ;
  EPioType ulPinType ;
  uint32_t ulPinConfiguration ;
  uint32_t ulPinAttribute ;
  EAnalogChannel ulAnalogChannel ; /* Analog pin in the Arduino context (label on the board) */
  EAnalogChannel ulADCChannelNumber ; /* ADC Channel number in the SAM device */
  EPWMChannel ulPWMChannel ;
  ETCChannel ulTCChannel ;
} PinDescription ;

PinDescription _g_APinDescription[];

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2
#define HIGH 0x1
#define LOW 0x0
#define digitalPinToPort(P) ( _g_APinDescription[P].pPort )
#define digitalPinToBitMask(P) ( _g_APinDescription[P].ulPin )

uint8_t g_pinStatus[];
#define PIN_STATUS_DIGITAL_INPUT_PULLUP  (0x01)
#define PIN_STATUS_DIGITAL_INPUT         (0x02)
#define PIN_STATUS_DIGITAL_OUTPUT        (0x03)
#define PIN_STATUS_ANALOG                (0x04)
#define PIN_STATUS_PWM                   (0x05)
#define PIN_STATUS_TIMER                 (0x06)
#define PIN_STATUS_SERIAL                (0x07)
#define PIN_STATUS_DW_LOW                (0x10)
#define PIN_STATUS_DW_HIGH               (0x11)

#define PIO_PULLUP  (1u << 0)
#define PIO_DEFAULT (0u << 0)
//#define ADC 0
#define PIN_ATTR_COMBO         (1UL<<0)
#define PIN_ATTR_ANALOG        (1UL<<1)
#define PIN_ATTR_DIGITAL       (1UL<<2)
#define PIN_ATTR_PWM           (1UL<<3)
#define PIN_ATTR_TIMER         (1UL<<4)

/*extern*/ void pinMode( uint32_t ulPin, uint32_t ulMode );

bool hasOffsetAddressMapping(void);
void initSS(W5100Class *w5100Class);
void setSS(W5100Class *w5100Class);
void resetSS(W5100Class *w5100Class);

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
