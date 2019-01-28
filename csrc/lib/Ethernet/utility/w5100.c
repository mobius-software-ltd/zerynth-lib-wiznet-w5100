#include "w5100.h"

PinDescription _g_APinDescription[];
/***************************************************/
/**            Default SS pin setting             **/
/***************************************************/

// If variant.h or other headers specifically define the
// default SS pin for ethernet, use it.
#if defined(PIN_SPI_SS_ETHERNET_LIB)
#define SS_PIN_DEFAULT  PIN_SPI_SS_ETHERNET_LIB
// If variant.h or other headers define these names
// use them if none of the other cases match
#elif defined(PIN_SPI_SS)
#define SS_PIN_DEFAULT  PIN_SPI_SS
#elif defined(CORE_SS0_PIN)
#define SS_PIN_DEFAULT  CORE_SS0_PIN
// As a final fallback, use pin 10
#else
#define SS_PIN_DEFAULT  10
#endif

uint8_t w5100ClassInit(W5100Class * w5100) {
	w5100 -> chip = 0;
	w5100 -> ss_pin = SS_PIN_DEFAULT;
  #ifdef ETHERNET_LARGE_BUFFERS
    w5100->SSIZE = 2048;
    w5100->SMASK = 0x07FF;
  #endif

  bool initialized = false;
	uint8_t i;
  delay(560);
  spiClassBeginNoPin(spiClass);
	initSS(w5100);
	resetSS(w5100);
	spiClassBeginTransactionNoPin(spiClass, SPI_ETHERNET_SETTINGS);

	w5100->CH_BASE_MSB = 0x04;
#ifdef ETHERNET_LARGE_BUFFERS
  #if MAX_SOCK_NUM <= 1
  		w5100->SSIZE = 8192;
  		writeTMSR(w5100, 0x03);
  		writeRMSR(w5100, 0x03);
  #elif MAX_SOCK_NUM <= 2
  		w5100->SSIZE = 4096;
  		writeTMSR(w5100, 0x0A);
  		writeRMSR(w5100, 0x0A);
  #else
  		w5100->SSIZE = 2048;
  		writeTMSR(w5100, 0x55);
  		writeRMSR(w5100, 0x55);
  #endif
  		w5100->SMASK = SSIZE - 1;
#else
		writeTMSR(w5100, 0x55);
		writeRMSR(w5100, 0x55);
#endif

	spiClassEndTransaction(spiClass);
	initialized = true;
	return 1; // successful init
}

void w5100ClassSetGatewayIp(W5100Class *w5100Class, const uint8_t * addr) { writeGAR(w5100Class, addr);}
void w5100ClassGetGatewayIp(W5100Class *w5100Class, uint8_t * addr) { readGAR(w5100Class, addr); }
void w5100ClassSetSubnetMask(W5100Class *w5100Class, const uint8_t * addr) { writeSUBR(w5100Class, addr); }
void w5100ClassGetSubnetMask(W5100Class *w5100Class, uint8_t * addr) { readSUBR(w5100Class, addr); }
void w5100ClassSetMACAddress(W5100Class *w5100Class, const uint8_t * addr) { writeSHAR(w5100Class, addr); }
void w5100ClassGetMACAddress(W5100Class *w5100Class, uint8_t * addr) { readSHAR(w5100Class, addr); }
void w5100ClassSetIPAddress(W5100Class *w5100Class, const uint8_t * addr) { writeSIPR(w5100Class, addr); }
void w5100ClassGetIPAddress(W5100Class *w5100Class, uint8_t * addr) { readSIPR(w5100Class, addr); }
void w5100ClassSetRetransmissionTime(W5100Class *w5100Class, uint16_t timeout) { writeRTR(w5100Class, timeout); }
void w5100ClassSetRetransmissionCount(W5100Class *w5100Class, uint8_t retry) { writeRCR(w5100Class, retry); }

// Soft reset the Wiznet chip, by writing to its MR register reset bit
uint8_t softReset(W5100Class *w5100Class)
{
	uint16_t count=0;

	// write to reset bit
	writeMR(w5100Class, 0x80);
	// then wait for soft reset to complete
	do {
		uint8_t mr = readMR(w5100Class);
		if (mr == 0) return 1;
		delay(1);
	} while (++count < 20);
	return 0;
}

uint8_t isW5100(W5100Class *w5100Class)
{
	w5100Class->chip = 51;
	if (!softReset(w5100Class)) return 0;
	writeMR(w5100Class, 0x10);
	if (readMR(w5100Class) != 0x10) return 0;
	writeMR(w5100Class, 0x12);
	if (readMR(w5100Class) != 0x12) return 0;
	writeMR(w5100Class, 0x00);
	if (readMR(w5100Class) != 0x00) return 0;
	return 1;
}

W5100Linkstatus getLinkStatus(W5100Class *w5100Class)
{
	return UNKNOWN;
}

uint8_t writeByte(W5100Class *w5100Class, uint16_t addr, uint8_t data) { return writeBuffer(w5100Class, addr, &data, 1); }

uint16_t writeBuffer(W5100Class *w5100Class, uint16_t addr, const uint8_t *buf, uint16_t len)
{
	uint8_t cmd[8];

	for (uint16_t i=0; i<len; i++) {
		setSS(w5100Class);
		spiClassTransferNoPin(spiClass, 0xF0, SPI_LAST);
		spiClassTransferNoPin(spiClass, addr >> 8, SPI_LAST);
		spiClassTransferNoPin(spiClass, addr & 0xFF, SPI_LAST);
		addr++;
		spiClassTransferNoPin(spiClass, buf[i], SPI_LAST);
		resetSS(w5100Class);
	}
	return len;
}

uint8_t readByte(W5100Class *w5100Class, uint16_t addr) {
  uint8_t data;
  readBuffer(w5100Class, addr, &data, 1);
  return data;
}

uint16_t readBuffer(W5100Class *w5100Class, uint16_t addr, uint8_t *buf, uint16_t len)
{
	uint8_t cmd[4];
		for (uint16_t i=0; i < len; i++) {
			setSS(w5100Class);
			#if 1
      spiClassTransferNoPin(spiClass, 0xF0, SPI_LAST);
  		spiClassTransferNoPin(spiClass, addr >> 8, SPI_LAST);
  		spiClassTransferNoPin(spiClass, addr & 0xFF, SPI_LAST);
			addr++;
			buf[i] = spiClassTransferNoPin(spiClass, 0, SPI_LAST);
			#else
			cmd[0] = 0x0F;
			cmd[1] = addr >> 8;
			cmd[2] = addr & 0xFF;
			cmd[3] = 0;
			SPI.transfer(cmd, 4); // TODO: why doesn't this work?
			buf[i] = cmd[3];
			addr++;
			#endif
			resetSS(w5100Class);
		}

	return len;
}

// Mode
void writeMR(W5100Class *w5100Class, uint8_t _data) { writeByte(w5100Class, 0x0000, _data);}
uint8_t readMR(W5100Class *w5100Class) { return readByte(w5100Class, 0x0000);}
// Interrupt
void writeIR(W5100Class *w5100Class, uint8_t _data) { writeByte(w5100Class, 0x0015, _data);}
uint8_t readIR(W5100Class *w5100Class) { return readByte(w5100Class, 0x0015);}
// Interrupt Mask
void writeIMR(W5100Class *w5100Class, uint8_t _data) { writeByte(w5100Class, 0x0016, _data);}
uint8_t readIMR(W5100Class *w5100Class) { return readByte(w5100Class, 0x0016);}
// Retry count
void writeRCR(W5100Class *w5100Class, uint8_t _data) { writeByte(w5100Class, 0x0019, _data);}
uint8_t readRCR(W5100Class *w5100Class) { return readByte(w5100Class, 0x0019);}
// Receive memory size (W5100 only)
void writeRMSR(W5100Class *w5100Class, uint8_t _data) { writeByte(w5100Class, 0x001A, _data);}
uint8_t readRMSR(W5100Class *w5100Class) { return readByte(w5100Class, 0x001A);}
// Transmit memory size (W5100 only)
void writeTMSR(W5100Class *w5100Class, uint8_t _data) { writeByte(w5100Class, 0x001B, _data);}
uint8_t readTMSR(W5100Class *w5100Class) { return readByte(w5100Class, 0x001B);}
// Authentication type address in PPPoE mode
void writePATR(W5100Class *w5100Class, uint8_t _data) { writeByte(w5100Class, 0x001C, _data);}
uint8_t readPATR(W5100Class *w5100Class) { return readByte(w5100Class, 0x001C);}
// PPP LCP Request Timer
void writePTIMER(W5100Class *w5100Class, uint8_t _data) { writeByte(w5100Class, 0x0028, _data);}
uint8_t readPTIMER(W5100Class *w5100Class) { return readByte(w5100Class, 0x0028);}
// PPP LCP Magic Number
void writePMAGIC(W5100Class *w5100Class, uint8_t _data) { writeByte(w5100Class, 0x0029, _data);}
uint8_t readPMAGIC(W5100Class *w5100Class) { return readByte(w5100Class, 0x0029);}
// Timeout address
void writeRTR(W5100Class *w5100Class, uint16_t _data) {
  uint8_t buf[2];
  buf[0] = _data >> 8;
  buf[1] = _data & 0xFF;
  writeBuffer(w5100Class, 0x0017, buf, 2);
}
uint16_t readRTR(W5100Class *w5100Class) {
  uint8_t buf[2];
  readBuffer(w5100Class,0x0017, buf, 2);
  return (buf[0] << 8) | buf[1];
}
// Unreachable Port address in UDP mode (W5100 only)
void writeUPORT(W5100Class *w5100Class, uint16_t _data) {
  uint8_t buf[2];
  buf[0] = _data >> 8;
  buf[1] = _data & 0xFF;
  writeBuffer(w5100Class, 0x002E, buf, 2);
}
uint16_t readUPORT(W5100Class *w5100Class) {
  uint8_t buf[2];
  readBuffer(w5100Class,0x002E, buf, 2);
  return (buf[0] << 8) | buf[1];
}
// Gateway IP address
void writeGAR(W5100Class *w5100Class, const uint8_t * _buff) { return writeBuffer(w5100Class, 0x0001, _buff, 4);}
void readGAR(W5100Class *w5100Class, uint8_t *_buff) { return readBuffer(w5100Class, 0x0001, _buff, 4);}
// Subnet mask address
void writeSUBR(W5100Class *w5100Class, const uint8_t *_buff) { return writeBuffer(w5100Class, 0x0005, _buff, 4);}
void readSUBR(W5100Class *w5100Class, uint8_t *_buff) { return readBuffer(w5100Class, 0x0005, _buff, 4);}
// Source MAC address
void writeSHAR(W5100Class *w5100Class, const uint8_t *_buff) { return writeBuffer(w5100Class,0x0009, _buff, 6);}
void readSHAR(W5100Class *w5100Class, uint8_t *_buff) { return readBuffer(w5100Class, 0x0009, _buff, 6);}
// Source IP address
void writeSIPR(W5100Class *w5100Class, const uint8_t *_buff) { return writeBuffer(w5100Class,0x000F, _buff, 4);}
void readSIPR(W5100Class *w5100Class, uint8_t *_buff) { return readBuffer(w5100Class, 0x000F, _buff, 4);}
// Unreachable IP address in UDP mode (W5100 only)
void writeUIPR(W5100Class *w5100Class, const uint8_t *_buff) { return writeBuffer(w5100Class,0x002A, _buff, 4);}
void readUIPR(W5100Class *w5100Class, uint8_t *_buff) { return readBuffer(w5100Class, 0x002A, _buff, 4);}

uint16_t getCH_BASE(W5100Class *w5100Class) { return w5100Class->CH_BASE_MSB << 8;}

uint8_t readSn(W5100Class *w5100Class, SOCKET s, uint16_t addr) { return readByte(w5100Class, getCH_BASE(w5100Class) + s * CH_SIZE + addr);}
uint8_t writeSn(W5100Class *w5100Class, SOCKET s, uint16_t addr, uint8_t data) { return writeByte(w5100Class, getCH_BASE(w5100Class) + s * CH_SIZE + addr, data);}
uint16_t readSnBuffer(W5100Class *w5100Class, SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len) { return readBuffer(w5100Class, getCH_BASE(w5100Class) + s * CH_SIZE + addr, buf, len);}
uint16_t writeSnBuffer(W5100Class *w5100Class, SOCKET s, uint16_t addr, uint8_t *buf, uint16_t len) { return writeBuffer(w5100Class, getCH_BASE(w5100Class) + s * CH_SIZE + addr, buf, len);}

// Mode
void writeSnMR(W5100Class *w5100Class, SOCKET _s, uint8_t _data) { writeSn(w5100Class, _s, 0x0000, _data);}
uint8_t readSnMR(W5100Class *w5100Class, SOCKET _s) { return readSn(w5100Class, _s, 0x0000);}
// Command
void writeSnCR(W5100Class *w5100Class, SOCKET _s, uint8_t _data) { writeSn(w5100Class, _s, 0x0001, _data);}
uint8_t readSnCR(W5100Class *w5100Class, SOCKET _s) { return readSn(w5100Class, _s, 0x0001);}
// Interrupt
void writeSnIR(W5100Class *w5100Class, SOCKET _s, uint8_t _data) { writeSn(w5100Class, _s, 0x0002, _data);}
uint8_t readSnIR(W5100Class *w5100Class, SOCKET _s) { return readSn(w5100Class, _s, 0x0002);}
// Status
void writeSnSR(W5100Class *w5100Class, SOCKET _s, uint8_t _data) { writeSn(w5100Class, _s, 0x0003, _data);}
uint8_t readSnSR(W5100Class *w5100Class, SOCKET _s) { return readSn(w5100Class, _s, 0x0003);}
// Protocol in IP RAW Mode
void writeSnPROTO(W5100Class *w5100Class, SOCKET _s, uint8_t _data) { writeSn(w5100Class, _s, 0x0014, _data);}
uint8_t readSnPROTO(W5100Class *w5100Class, SOCKET _s) { return readSn(w5100Class, _s, 0x0014);}
// IP TOS
void writeSnTOS(W5100Class *w5100Class, SOCKET _s, uint8_t _data) { writeSn(w5100Class, _s, 0x0015, _data);}
uint8_t readSnTOS(W5100Class *w5100Class, SOCKET _s) { return readSn(w5100Class, _s, 0x0015);}
// IP TTL
void writeSnTTL(W5100Class *w5100Class, SOCKET _s, uint8_t _data) { writeSn(w5100Class, _s, 0x0016, _data);}
uint8_t readSnTTL(W5100Class *w5100Class, SOCKET _s) { return readSn(w5100Class, _s, 0x0016);}

// Source Port
void writeSnPORT(W5100Class *w5100Class, SOCKET _s, uint16_t _data) {
  uint8_t buf[2];
  buf[0] = _data >> 8;
  buf[1] = _data & 0xFF;
  writeSnBuffer(w5100Class, _s, 0x0004, buf, 2);
}
uint16_t readSnPORT(W5100Class *w5100Class, SOCKET _s) {
  uint8_t buf[2];
  readSnBuffer(w5100Class, _s, 0x0004, buf, 2);
  return (buf[0] << 8) | buf[1];
}
// Destination Port
void writeSnDPORT(W5100Class *w5100Class, SOCKET _s, uint16_t _data) {
  uint8_t buf[2];
  buf[0] = _data >> 8;
  buf[1] = _data & 0xFF;
  writeSnBuffer(w5100Class, _s, 0x0010, buf, 2);
}
uint16_t readSnDPORT(W5100Class *w5100Class, SOCKET _s) {
  uint8_t buf[2];
  readSnBuffer(w5100Class, _s, 0x0010, buf, 2);
  return (buf[0] << 8) | buf[1];
}
// Max Segment Size
void writeSnMSSR(W5100Class *w5100Class, SOCKET _s, uint16_t _data) {
  uint8_t buf[2];
  buf[0] = _data >> 8;
  buf[1] = _data & 0xFF;
  writeSnBuffer(w5100Class, _s, 0x0012, buf, 2);
}
uint16_t readSnMSSR(W5100Class *w5100Class, SOCKET _s) {
  uint8_t buf[2];
  readSnBuffer(w5100Class, _s, 0x0012, buf, 2);
  return (buf[0] << 8) | buf[1];
}
// TX Free Size
void writeSnTX_FSR(W5100Class *w5100Class, SOCKET _s, uint16_t _data) {
  uint8_t buf[2];
  buf[0] = _data >> 8;
  buf[1] = _data & 0xFF;
  writeSnBuffer(w5100Class, _s, 0x0020, buf, 2);
}
uint16_t readSnTX_FSR(W5100Class *w5100Class, SOCKET _s) {
  uint8_t buf[2];
  readSnBuffer(w5100Class, _s, 0x0020, buf, 2);
  return (buf[0] << 8) | buf[1];
}
// TX Write Pointer
void writeSnTX_WR(W5100Class *w5100Class, SOCKET _s, uint16_t _data) {
  uint8_t buf[2];
  buf[0] = _data >> 8;
  buf[1] = _data & 0xFF;
  writeSnBuffer(w5100Class, _s, 0x0024, buf, 2);
}
uint16_t readSnTX_WR(W5100Class *w5100Class, SOCKET _s) {
  uint8_t buf[2];
  readSnBuffer(w5100Class, _s, 0x0024, buf, 2);
  return (buf[0] << 8) | buf[1];
}
// RX Free Size
void writeSnRX_RSR(W5100Class *w5100Class, SOCKET _s, uint16_t _data) {
  uint8_t buf[2];
  buf[0] = _data >> 8;
  buf[1] = _data & 0xFF;
  writeSnBuffer(w5100Class, _s, 0x0026, buf, 2);
}
uint16_t readSnRX_RSR(W5100Class *w5100Class, SOCKET _s) {
  uint8_t buf[2];
  readSnBuffer(w5100Class, _s, 0x0026, buf, 2);
  return (buf[0] << 8) | buf[1];
}
// RX Read Pointer
void writeSnRX_RD(W5100Class *w5100Class, SOCKET _s, uint16_t _data) {
  uint8_t buf[2];
  buf[0] = _data >> 8;
  buf[1] = _data & 0xFF;
  writeSnBuffer(w5100Class, _s, 0x0028, buf, 2);
}
uint16_t readSnRX_RD(W5100Class *w5100Class, SOCKET _s) {
  uint8_t buf[2];
  readSnBuffer(w5100Class, _s, 0x0028, buf, 2);
  return (buf[0] << 8) | buf[1];
}
// RX Write Pointer (supported?)
void writeSnRX_WR(W5100Class *w5100Class,SOCKET _s, uint16_t _data) {
  uint8_t buf[2];
  buf[0] = _data >> 8;
  buf[1] = _data & 0xFF;
  writeSnBuffer(w5100Class, _s, 0x002A, buf, 2);
}
uint16_t readSnRX_WR(W5100Class *w5100Class,SOCKET _s) {
  uint8_t buf[2];
  readSnBuffer(w5100Class, _s, 0x002A, buf, 2);
  return (buf[0] << 8) | buf[1];
}
// Destination Hardw Addr
uint16_t writeSnDHAR(W5100Class *w5100Class,SOCKET _s, uint8_t *_buff) { return writeSnBuffer(w5100Class, _s, 0x0006, _buff, 6);}
uint16_t readSnDHAR(W5100Class *w5100Class,SOCKET _s, uint8_t *_buff) { return readSnBuffer(w5100Class, _s, 0x0006, _buff, 6);}
// Destination IP Addr
uint16_t writeSnDIPR(W5100Class *w5100Class,SOCKET _s, uint8_t *_buff) { return writeSnBuffer(w5100Class, _s, 0x000C, _buff, 4);}
uint16_t readSnDIPR(W5100Class *w5100Class,SOCKET _s, uint8_t *_buff) { return readSnBuffer(w5100Class, _s, 0x000C, _buff, 4);}

uint8_t getChip(W5100Class *w5100Class) { return w5100Class->chip; }

uint16_t SBASE(W5100Class *w5100Class, uint8_t socknum) {
  if (w5100Class->chip == 51) {
    return socknum * w5100Class->SSIZE + 0x4000;
  } else {
    return socknum * w5100Class->SSIZE + 0x8000;
  }
}

uint16_t RBASE(W5100Class *w5100Class, uint8_t socknum) {
  if (w5100Class -> chip == 51) {
    return socknum * w5100Class->SSIZE + 0x6000;
  } else {
    return socknum * w5100Class->SSIZE + 0xC000;
  }
}

/*extern*/ void pinMode( uint32_t ulPin, uint32_t ulMode )
{
	if ( _g_APinDescription[ulPin].ulPinType == PIO_NOT_A_PIN )
    {
        return ;
    }

  if ((g_pinStatus[ulPin] & 0xF) == PIN_STATUS_ANALOG)
    {
      adc_disable_channel( ADC, _g_APinDescription[ulPin].ulADCChannelNumber);
    }

  if ((g_pinStatus[ulPin] & 0xF) < PIN_STATUS_DIGITAL_OUTPUT && g_pinStatus[ulPin] != 0)
    {
      // return if already configured in the right way
      if (((g_pinStatus[ulPin] & 0xF) == PIN_STATUS_DIGITAL_INPUT && ulMode == INPUT) ||
          ((g_pinStatus[ulPin] & 0xF) == PIN_STATUS_DIGITAL_INPUT_PULLUP && ulMode == INPUT_PULLUP) ||
          ((g_pinStatus[ulPin] & 0xF) == PIN_STATUS_DIGITAL_OUTPUT && ulMode == OUTPUT))
      return;
    }

	switch ( ulMode )
    {
        case INPUT:
            /* Enable peripheral for clocking input */
            pmc_enable_periph_clk( _g_APinDescription[ulPin].ulPeripheralId ) ;
            PIO_Configure(
            	_g_APinDescription[ulPin].pPort,
            	PIO_INPUT,
            	_g_APinDescription[ulPin].ulPin,
            	0 ) ;
            g_pinStatus[ulPin] = (g_pinStatus[ulPin] & 0xF0) | PIN_STATUS_DIGITAL_INPUT;
        break ;

        case INPUT_PULLUP:
            /* Enable peripheral for clocking input */
            pmc_enable_periph_clk( _g_APinDescription[ulPin].ulPeripheralId ) ;
            PIO_Configure(
            	_g_APinDescription[ulPin].pPort,
            	PIO_INPUT,
            	_g_APinDescription[ulPin].ulPin,
            	PIO_PULLUP ) ;
            g_pinStatus[ulPin] = (g_pinStatus[ulPin] & 0xF0) | PIN_STATUS_DIGITAL_INPUT_PULLUP;
        break ;

        case OUTPUT:
            PIO_Configure(
            	_g_APinDescription[ulPin].pPort,
              (g_pinStatus[ulPin] & 0xF0) >> 4 ? PIO_OUTPUT_1 : PIO_OUTPUT_0,
            	_g_APinDescription[ulPin].ulPin,
            	_g_APinDescription[ulPin].ulPinConfiguration ) ;

            g_pinStatus[ulPin] = (g_pinStatus[ulPin] & 0xF0) | PIN_STATUS_DIGITAL_OUTPUT;

            /* if all pins are output, disable PIO Controller clocking, reduce power consumption */
            if ( _g_APinDescription[ulPin].pPort->PIO_OSR == 0xffffffff )
            {
                pmc_disable_periph_clk( _g_APinDescription[ulPin].ulPeripheralId ) ;
            }
        break ;

        default:
        break ;
    }
}

bool hasOffsetAddressMapping(void) { return false;}
void initSS(W5100Class *w5100Class) {
		w5100Class->ss_pin_reg = &(digitalPinToPort(w5100Class->ss_pin)->PIO_PER);
		w5100Class->ss_pin_mask = digitalPinToBitMask(w5100Class->ss_pin);
		pinMode(w5100Class->ss_pin, OUTPUT);
}
void setSS(W5100Class *w5100Class) {*(w5100Class->ss_pin_reg+13) = w5100Class->ss_pin_mask;}
void resetSS(W5100Class *w5100Class) {*(w5100Class->ss_pin_reg+12) = w5100Class->ss_pin_mask;}

void w5100ClassExecCmdSn(W5100Class *w5100Class, SOCKET s, SockCMD _cmd)
{
	// Send command to socket
	writeSnCR(w5100Class, s, _cmd);
	// Wait for command to complete
	while (readSnCR(w5100Class, s)) ;
}
