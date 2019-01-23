#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED
#include "inttypes.h"
#include "variant.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_HAS_TRANSACTION 1
#define SPI_HAS_EXTENDED_CS_PIN_HANDLING 1

#define SPI_MODE0 0x02
#define SPI_MODE1 0x00
#define SPI_MODE2 0x03
#define SPI_MODE3 0x01

typedef enum  {
	LSBFIRST = 0,
	MSBFIRST = 1
} BitOrder;

typedef enum {
	SPI_CONTINUE,
	SPI_LAST
} SPITransferMode;

typedef struct _SPISettings {
  uint32_t config;
	BitOrder border;
} SPISettings;

int spiSettingsConstructorParams(SPISettings * spiSettings, uint32_t clock, BitOrder bitOrder, uint8_t dataMode);
int spiSettingsConstructor(SPISettings * spiSettings);

#define F_CPU 84000000L
#define SPI_CSR_CSAAT (0x1u << 3)
#define SPI_CSR_SCBR_Pos 8
#define SPI_CSR_SCBR_Msk (0xffu << SPI_CSR_SCBR_Pos)
#define SPI_CSR_SCBR(value) ((SPI_CSR_SCBR_Msk & ((value) << SPI_CSR_SCBR_Pos)))
#define SPI_CSR_DLYBCT_Pos 24
#define SPI_CSR_DLYBCT_Msk (0xffu << SPI_CSR_DLYBCT_Pos)
#define SPI_CSR_DLYBCT(value) ((SPI_CSR_DLYBCT_Msk & ((value) << SPI_CSR_DLYBCT_Pos)))

void init_MightInline(SPISettings * spiSettings, uint32_t clock, BitOrder bitOrder, uint8_t dataMode);
void inline init_AlwaysInline(SPISettings * spiSettings, uint32_t clock, BitOrder bitOrder, uint8_t dataMode);

typedef struct {
  WoReg SPI_CR;        /**< \brief (Spi Offset: 0x00) Control Register */
  RwReg SPI_MR;        /**< \brief (Spi Offset: 0x04) Mode Register */
  RoReg SPI_RDR;       /**< \brief (Spi Offset: 0x08) Receive Data Register */
  WoReg SPI_TDR;       /**< \brief (Spi Offset: 0x0C) Transmit Data Register */
  RoReg SPI_SR;        /**< \brief (Spi Offset: 0x10) Status Register */
  WoReg SPI_IER;       /**< \brief (Spi Offset: 0x14) Interrupt Enable Register */
  WoReg SPI_IDR;       /**< \brief (Spi Offset: 0x18) Interrupt Disable Register */
  RoReg SPI_IMR;       /**< \brief (Spi Offset: 0x1C) Interrupt Mask Register */
  RoReg Reserved1[4];
  RwReg SPI_CSR[4];    /**< \brief (Spi Offset: 0x30) Chip Select Register */
  RoReg Reserved2[41];
  RwReg SPI_WPMR;      /**< \brief (Spi Offset: 0xE4) Write Protection Control Register */
  RoReg SPI_WPSR;      /**< \brief (Spi Offset: 0xE8) Write Protection Status Register */
} Spi;

typedef struct _SPIClass {
	Spi *spi;
	uint32_t id;
	BitOrder bitOrder[SPI_CHANNELS_NUM];
	uint32_t divider[SPI_CHANNELS_NUM];
	uint32_t mode[SPI_CHANNELS_NUM];
	void (*initCb)(void);
	bool initialized;
	uint8_t interruptMode;    // 0=none, 1-15=mask, 16=global
	uint8_t interruptSave;    // temp storage, to restore state
	uint32_t interruptMask[4];
} SPIClass;

spiClassConstructor(SPIClass * spiClass, Spi *_spi, uint32_t _id/*, void(*_initCb)(void)*/);
uint8_t spiClassTransfer(SPIClass * spiClass, uint8_t _pin, uint8_t _data, SPITransferMode _mode);
uint16_t spiClassTransfer16(SPIClass * spiClass, uint8_t _pin, uint16_t _data, SPITransferMode _mode);
void spiClassTransferBuffer(SPIClass * spiClass, uint8_t _pin, void *_buf, size_t _count, SPITransferMode _mode);
uint8_t spiClassTransferNoPin(SPIClass * spiClass, uint8_t _data, SPITransferMode _mode);
uint16_t spiClassTransfer16NoPin(SPIClass * spiClass, uint16_t _data, SPITransferMode _mode);
void spiClassTransferBufferNoPin(SPIClass * spiClass, void *_buf, size_t _count, SPITransferMode _mode);
void spiClassUsingInterrupt(SPIClass * spiClass, uint8_t interruptNumber);
void spiClassBeginTransactionNoPin(SPIClass * spiClass, SPISettings * settings);
void spiClassBeginTransaction(SPIClass * spiClass, uint8_t pin, SPISettings  * settings);
void spiClassEndTransaction(SPIClass * spiClass);
void spiClassAttachInterrupt(SPIClass * spiClass);
void spiClassDetachInterrupt(SPIClass * spiClass);
void spiClassBeginNoPin(SPIClass * spiClass);
void spiClassEndNoPin(SPIClass * spiClass);
void spiClassBegin(SPIClass * spiClass, uint8_t _pin);
void spiClassEnd(SPIClass * spiClass, uint8_t _pin);
void spiClassSetBitOrder(SPIClass * spiClass, uint8_t _pin, BitOrder);
void spiClassSetDataMode(SPIClass * spiClass, uint8_t _pin, uint8_t);
void spiClassSetClockDivider(SPIClass * spiClass, uint8_t _pin, uint8_t);
void spiClassSetBitOrderNoPin(SPIClass * spiClass, BitOrder _order);
void spiClassSetDataModeNoPin(SPIClass * spiClass, uint8_t _mode);
void spiClassSetClockDividerNoPin(SPIClass * spiClass, uint8_t _div);
void spiClassInit(SPIClass * spiClass);
// For compatibility with sketches designed for AVR @ 16 MHz
// New programs should use SPI.beginTransaction to set the SPI clock
#define SPI_CLOCK_DIV2	 11
#define SPI_CLOCK_DIV4	 21
#define SPI_CLOCK_DIV8	 42
#define SPI_CLOCK_DIV16	 84
#define SPI_CLOCK_DIV32	 168
#define SPI_CLOCK_DIV64	 255
#define SPI_CLOCK_DIV128 255

#define SPI_MR_MSTR (0x1u << 0)
#define SPI_MR_PS (0x1u << 1)
#define SPI_MR_MODFDIS (0x1u << 4)

#define interrupts() __enable_irq()
#define noInterrupts() __disable_irq()

#define SPI_TDR_LASTXFER (0x1u << 24)
#define SPI_SR_TDRE (0x1u << 1)
#define SPI_SR_RDRF (0x1u << 0)
#define SPI_TDR_LASTXFER (0x1u << 24)

#endif
