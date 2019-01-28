#include "SPI.h"

#if ARDUINO >= 156 && !defined(ARDUINO_ARCH_PIC32)
extern void yield(void);
#else
#define yield()
#endif

inline void __enable_irq(void)
{
  __asm__ volatile ("cpsie i");
}
inline void __disable_irq(void)
{
  __asm__ volatile ("cpsid i");
}
static volatile uint32_t _dwTickCount=0;

SPIClass * spiClass;
SPISettings * spiSettings;

void SPI_0_Init(void) {
	PIO_Configure(
			g_APinDescription[PIN_SPI_MOSI].pPort,
			g_APinDescription[PIN_SPI_MOSI].ulPinType,
			g_APinDescription[PIN_SPI_MOSI].ulPin,
			g_APinDescription[PIN_SPI_MOSI].ulPinConfiguration);
	PIO_Configure(
			g_APinDescription[PIN_SPI_MISO].pPort,
			g_APinDescription[PIN_SPI_MISO].ulPinType,
			g_APinDescription[PIN_SPI_MISO].ulPin,
			g_APinDescription[PIN_SPI_MISO].ulPinConfiguration);
	PIO_Configure(
			g_APinDescription[PIN_SPI_SCK].pPort,
			g_APinDescription[PIN_SPI_SCK].ulPinType,
			g_APinDescription[PIN_SPI_SCK].ulPin,
			g_APinDescription[PIN_SPI_SCK].ulPinConfiguration);
}

void adc_disable_channel(Adc *p_adc, const enum adc_channel_num_t adc_ch)
{
	p_adc->ADC_CHDR = 1 << adc_ch;
}

void init_MightInline(SPISettings * spiSettings, uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
	init_AlwaysInline(spiSettings, clock, bitOrder, dataMode);
}

void inline init_AlwaysInline(SPISettings * spiSettings, uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
	spiSettings -> border = bitOrder;
	uint8_t div;
	if (clock < (F_CPU / 255)) {
		div = 255;
	} else if (clock >= (F_CPU / 2)) {
		div = 2;
	} else {
		div = (F_CPU / (clock + 1)) + 1;
	}
	spiSettings -> config = (dataMode & 3) | SPI_CSR_CSAAT | SPI_CSR_SCBR(div) | SPI_CSR_DLYBCT(1);
}

int piSettingsConstructorParams(SPISettings * spiSettings, uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
  if (__builtin_constant_p(clock)) {
    init_AlwaysInline(spiSettings, clock, bitOrder, dataMode);
  } else {
    init_MightInline(spiSettings, clock, bitOrder, dataMode);
  }
	return 0;
}

int spiSettingsConstructor(SPISettings * spiSettings) {
	init_AlwaysInline(spiSettings, 14000000, MSBFIRST, SPI_MODE0);
	return 0;
}

spiClassConstructor(SPIClass * spiClass, Spi *_spi, uint32_t _id/*, void(*_initCb)(void)*/) {
	spiClass -> spi = _spi;
  spiClass -> id = _id;
  spiClass -> initCb = SPI_0_Init;
  spiClass -> initialized = false;
}

void spiClassBeginNoPin(SPIClass * spiClass) {
	spiClassInit(spiClass);
	// NPCS control is left to the user
	// Default speed set to 4Mhz
	spiClassSetClockDivider(spiClass, BOARD_SPI_DEFAULT_SS, 21);
	spiClassSetDataMode(spiClass, BOARD_SPI_DEFAULT_SS, SPI_MODE0);
	spiClassSetBitOrder(spiClass, BOARD_SPI_DEFAULT_SS, MSBFIRST);
}

void spiClassBegin(SPIClass * spiClass, uint8_t _pin) {
	spiClassInit(spiClass);

	uint32_t spiPin = BOARD_PIN_TO_SPI_PIN(_pin);
	PIO_Configure(
		g_APinDescription[spiPin].pPort,
		g_APinDescription[spiPin].ulPinType,
		g_APinDescription[spiPin].ulPin,
		g_APinDescription[spiPin].ulPinConfiguration);

	// Default speed set to 4Mhz
	spiClassSetClockDivider(spiClass, _pin, 21);
	spiClassSetDataMode(spiClass, _pin, SPI_MODE0);
	spiClassSetBitOrder(spiClass, _pin, MSBFIRST);
}

uint32_t pmc_enable_periph_clk(uint32_t ul_id)
{
	if (ul_id > MAX_PERIPH_ID) {
		return 1;
	}

	if (ul_id < 32) {
		if ((PMC->PMC_PCSR0 & (1u << ul_id)) != (1u << ul_id)) {
			PMC->PMC_PCER0 = 1 << ul_id;
		}
#if (SAM3S_SERIES || SAM3XA_SERIES || SAM4S_SERIES)
	} else {
		ul_id -= 32;
		if ((PMC->PMC_PCSR1 & (1u << ul_id)) != (1u << ul_id)) {
			PMC->PMC_PCER1 = 1 << ul_id;
		}
#endif
	}

	return 0;
}

uint32_t pmc_disable_periph_clk(uint32_t ul_id)
{
	if (ul_id > MAX_PERIPH_ID) {
		return 1;
	}

	if (ul_id < 32) {
		if ((PMC->PMC_PCSR0 & (1u << ul_id)) == (1u << ul_id)) {
			PMC->PMC_PCDR0 = 1 << ul_id;
		}
#if (SAM3S_SERIES || SAM3XA_SERIES || SAM4S_SERIES)
	} else {
		ul_id -= 32;
		if ((PMC->PMC_PCSR1 & (1u << ul_id)) == (1u << ul_id)) {
			PMC->PMC_PCDR1 = 1 << ul_id;
		}
#endif
	}
	return 0;
}


void SPI_Configure( Spi* spi, uint32_t dwId, uint32_t dwConfiguration )
{
    pmc_enable_periph_clk( dwId ) ;
    spi->SPI_CR = SPI_CR_SPIDIS ;

    /* Execute a software reset of the SPI twice */
    spi->SPI_CR = SPI_CR_SWRST ;
    spi->SPI_CR = SPI_CR_SWRST ;
    spi->SPI_MR = dwConfiguration ;
}

void SPI_Enable( Spi* spi )
{
    spi->SPI_CR = SPI_CR_SPIEN ;
}

void spiClassInit(SPIClass * spiClass) {
	if (spiClass -> initialized)
		return;
	spiClass -> interruptMode = 0;
	spiClass -> interruptSave = 0;
	spiClass -> interruptMask[0] = 0;
	spiClass -> interruptMask[1] = 0;
	spiClass -> interruptMask[2] = 0;
	spiClass -> interruptMask[3] = 0;
	spiClass -> initCb();
	SPI_Configure(spiClass -> spi, spiClass -> id, SPI_MR_MSTR | SPI_MR_PS | SPI_MR_MODFDIS);
	SPI_Enable(spiClass -> spi);
	spiClass -> initialized = true;
}

#ifndef interruptsStatus
#define interruptsStatus() __interruptsStatus()
inline unsigned char __interruptsStatus(void) __attribute__((always_inline, unused));
inline unsigned char __interruptsStatus(void) {
	unsigned int primask, faultmask;
	__asm__ volatile ("mrs %0, primask" : "=r" (primask));
	if (primask) return 0;
	__asm__ volatile ("mrs %0, faultmask" : "=r" (faultmask));
	if (faultmask) return 0;
	return 1;
}
#endif

void spiClassUsingInterrupt(SPIClass * spiClass, uint8_t interruptNumber)
{
	uint8_t irestore;

	irestore = interruptsStatus();
	noInterrupts();
	if (spiClass -> interruptMode < 16) {
		if (interruptNumber > NUM_DIGITAL_PINS) {
			spiClass -> interruptMode = 16;
		} else {
			Pio *pio = g_APinDescription[interruptNumber].pPort;
			uint32_t mask = g_APinDescription[interruptNumber].ulPin;
			if (pio == PIOA) {
				spiClass -> interruptMode |= 1;
				spiClass -> interruptMask[0] |= mask;
			} else if (pio == PIOB) {
				spiClass -> interruptMode |= 2;
				spiClass -> interruptMask[1] |= mask;
			} else if (pio == PIOC) {
				spiClass -> interruptMode |= 4;
				spiClass -> interruptMask[2] |= mask;
			} else if (pio == PIOD) {
				spiClass -> interruptMode |= 8;
				spiClass -> interruptMask[3] |= mask;
			} else {
				spiClass -> interruptMode = 16;
			}
		}
	}
	if (irestore) interrupts();
}

void spiClassConfigureNPCS( Spi *spi, uint32_t dwNpcs, uint32_t dwConfiguration )
{
    spi->SPI_CSR[dwNpcs] = dwConfiguration ;
}

extern uint32_t GetTickCount( void )
{
    return _dwTickCount ;
}

uint32_t millis( void )
{
// todo: ensure no interrupts
    return GetTickCount() ;
}

void delay( uint32_t ms )
{
    if (ms == 0)
        return;
    uint32_t start = GetTickCount();
    do {
        yield();
    } while (GetTickCount() - start < ms);
}

void spiClassBeginTransactionNoPin(SPIClass * spiClass, SPISettings * settings) { spiClassBeginTransaction(spiClass, BOARD_SPI_DEFAULT_SS, settings); }

void spiClassBeginTransaction(SPIClass * spiClass, uint8_t pin, SPISettings * settings)
{
	uint8_t mode = spiClass -> interruptMode;
	if (mode > 0) {
		if (mode < 16) {
      if (mode & 1) PIOA -> PIO_IDR = spiClass -> interruptMask[0];
			if (mode & 2) PIOB -> PIO_IDR = spiClass -> interruptMask[1];
			if (mode & 4) PIOC -> PIO_IDR = spiClass -> interruptMask[2];
			if (mode & 8) PIOD -> PIO_IDR = spiClass -> interruptMask[3];
    } else {
			spiClass -> interruptSave = interruptsStatus();
			noInterrupts();
		}
	}
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(pin);
	spiClass -> bitOrder[ch] = settings->border;
	spiClassConfigureNPCS(spiClass -> spi, ch, settings->config);
}

void spiClassEndTransaction(SPIClass * spiClass)
{
	uint8_t mode = spiClass -> interruptMode;
	if (mode > 0) {
		if (mode < 16) {
			if (mode & 1) PIOA->PIO_IER = spiClass ->interruptMask[0];
			if (mode & 2) PIOB->PIO_IER = spiClass ->interruptMask[1];
			if (mode & 4) PIOC->PIO_IER = spiClass ->interruptMask[2];
			if (mode & 8) PIOD->PIO_IER = spiClass ->interruptMask[3];
    } else {
			if (spiClass -> interruptSave) interrupts();
		}
	}
}

void spiClassEnd(SPIClass * spiClass, uint8_t _pin) {
	uint32_t spiPin = BOARD_PIN_TO_SPI_PIN(_pin);
	// Setting the pin as INPUT will disconnect it from SPI peripheral
	pinMode(spiPin, INPUT);
}

void SPI_Disable( Spi* spi )
{
    spi->SPI_CR = SPI_CR_SPIDIS ;
}

void spiClassEndNoPin(SPIClass * spiClass) {
	SPI_Disable(spiClass -> spi);
	spiClass -> initialized = false;
}

void spiClassSetBitOrder(SPIClass * spiClass, uint8_t _pin, BitOrder _bitOrder) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	spiClass -> bitOrder[ch] = _bitOrder;
}

void spiClassSetDataMode(SPIClass * spiClass, uint8_t _pin, uint8_t _mode) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	spiClass -> mode[ch] = _mode | SPI_CSR_CSAAT;
	// SPI_CSR_DLYBCT(1) keeps CS enabled for 32 MCLK after a completed
	// transfer. Some device needs that for working properly.
	spiClassConfigureNPCS(spiClass -> spi, ch, spiClass ->mode[ch] | SPI_CSR_SCBR(spiClass -> divider[ch]) | SPI_CSR_DLYBCT(1));
}

void spiClassSetClockDivider(SPIClass * spiClass, uint8_t _pin, uint8_t _divider) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	spiClass -> divider[ch] = _divider;
	// SPI_CSR_DLYBCT(1) keeps CS enabled for 32 MCLK after a completed
	// transfer. Some device needs that for working properly.
	spiClassConfigureNPCS(spiClass -> spi, ch, spiClass -> mode[ch] | SPI_CSR_SCBR(spiClass -> divider[ch]) | SPI_CSR_DLYBCT(1));
}

// These methods sets the same parameters but on default pin BOARD_SPI_DEFAULT_SS
void spiClassSetBitOrderNoPin(SPIClass * spiClass, BitOrder _order) { spiClassSetBitOrder(spiClass, BOARD_SPI_DEFAULT_SS, _order); }
void spiClassSetDataModeNoPin(SPIClass * spiClass, uint8_t _mode) { spiClassSetDataMode(spiClass, BOARD_SPI_DEFAULT_SS, _mode); }
void spiClassSetClockDividerNoPin(SPIClass * spiClass, uint8_t _div) { spiClassSetClockDivider(spiClass, BOARD_SPI_DEFAULT_SS, _div); }

uint32_t __RBIT(uint32_t value)
{
  uint32_t result;

   __asm__ volatile ("rbit %0, %1" : "=r" (result) : "r" (value) );
   return(result);
}

uint32_t __REV(uint32_t value)
{
  uint32_t result;

  __asm__ volatile ("rev %0, %1" : "=r" (result) : "r" (value) );
  return(result);
}

uint8_t spiClassTransfer(SPIClass * spiClass, uint8_t _pin, uint8_t _data, SPITransferMode _mode) {
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	// Reverse bit order
	if (spiClass->bitOrder[ch] == LSBFIRST)
		_data = __REV(__RBIT(_data));
	uint32_t d = _data | SPI_PCS(ch);
	if (_mode == SPI_LAST)
		d |= SPI_TDR_LASTXFER;

	// SPI_Write(spi, _channel, _data);
	while ((spiClass -> spi -> SPI_SR & SPI_SR_TDRE) == 0)
		;
	spiClass->spi->SPI_TDR = d;

	// return SPI_Read(spi);
	while ((spiClass ->spi->SPI_SR & SPI_SR_RDRF) == 0)
		;
	d = spiClass ->spi->SPI_RDR;
	// Reverse bit order
	if (spiClass ->bitOrder[ch] == LSBFIRST)
		d = __REV(__RBIT(d));
	return d & 0xFF;
}

uint16_t spiClassTransfer16(SPIClass * spiClass, uint8_t _pin, uint16_t _data, SPITransferMode _mode) {
	union { uint16_t val; struct { uint8_t lsb; uint8_t msb; }; } t;
	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);

	t.val = _data;

	if (spiClass->bitOrder[ch] == LSBFIRST) {
		t.lsb = spiClassTransfer(spiClass, _pin, t.lsb, SPI_CONTINUE);
		t.msb = spiClassTransfer(spiClass, _pin, t.msb, _mode);
	} else {
		t.msb = spiClassTransfer(spiClass, _pin, t.msb, SPI_CONTINUE);
		t.lsb = spiClassTransfer(spiClass, _pin, t.lsb, _mode);
	}

	return t.val;
}

void spiClassTransferBuffer(SPIClass * spiClass, uint8_t _pin, void *_buf, size_t _count, SPITransferMode _mode) {
	if (_count == 0)
		return;

	uint8_t *buffer = (uint8_t *)_buf;
	if (_count == 1) {
		*buffer = spiClassTransfer(spiClass, _pin, *buffer, _mode);
		return;
	}

	uint32_t ch = BOARD_PIN_TO_SPI_CHANNEL(_pin);
	bool reverse = (spiClass ->bitOrder[ch] == LSBFIRST);

	// Send the first byte
	uint32_t d = *buffer;
	if (reverse)
		d = __REV(__RBIT(d));
	while ((spiClass ->spi->SPI_SR & SPI_SR_TDRE) == 0);
	spiClass ->spi->SPI_TDR = d | SPI_PCS(ch);

	while (_count > 1) {
		// Prepare next byte
		d = *(buffer+1);
		if (reverse)
			d = __REV(__RBIT(d));
		if (_count == 2 && _mode == SPI_LAST)
			d |= SPI_TDR_LASTXFER;

		// Read transferred byte and send next one straight away
		while ((spiClass->spi->SPI_SR & SPI_SR_RDRF) == 0)
			;
		uint8_t r = spiClass->spi->SPI_RDR;
		spiClass->spi->SPI_TDR = d | SPI_PCS(ch);

		// Save read byte
		if (reverse)
			r = __REV(__RBIT(r));
		*buffer = r;
		buffer++;
		_count--;
	}

	// Receive the last transferred byte
	while ((spiClass->spi->SPI_SR & SPI_SR_RDRF) == 0)
		;
	uint8_t r = spiClass->spi->SPI_RDR;
	if (reverse)
		r = __REV(__RBIT(r));
	*buffer = r;
}

uint8_t spiClassTransferNoPin(SPIClass * spiClass, uint8_t _data, SPITransferMode _mode) { return spiClassTransfer(spiClass, BOARD_SPI_DEFAULT_SS, _data, _mode); }
uint16_t spiClassTransfer16NoPin(SPIClass * spiClass, uint16_t _data, SPITransferMode _mode) { return spiClassTransfer16(spiClass, BOARD_SPI_DEFAULT_SS, _data, _mode); }
void spiClassTransferBufferNoPin(SPIClass * spiClass, void *_buf, size_t _count, SPITransferMode _mode) { spiClassTransferBuffer(spiClass, BOARD_SPI_DEFAULT_SS, _buf, _count, _mode); }

void spiClassAttachInterrupt(SPIClass * spiClass) {
	// Should be enableInterrupt()
}

void spiClassDetachInterrupt(SPIClass * spiClass) {
	// Should be disableInterrupt()
}

//SPIClass SPI;
//spiClassConstructor(SPI, SPI_INTERFACE, SPI_INTERFACE_ID, SPI_0_Init);
