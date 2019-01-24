#ifndef _VARIANT_ARDUINO_DUE_X_
#define _VARIANT_ARDUINO_DUE_X_

#include "inttypes.h"
#include <stdio.h>
#include "system_Device.h"
#ifndef utils_h
#include "../cores/utils.h"

#endif
#ifndef _SAM3X8E_
#include "sam3x8e.h"
#endif
/*
#ifndef _SAM3N_PIO_COMPONENT_
#include "../Ethernet/utility/component_pio.h"
#endif
*/
/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1
#define SPI_INTERFACE       SPI0
#define SPI_INTERFACE_ID    ID_SPI0
#define SPI_CHANNELS_NUM 		 4
#define PIN_SPI_SS0          (77u)
#define PIN_SPI_SS1          (87u)
#define PIN_SPI_SS2          (86u)
#define PIN_SPI_SS3          (78u)
#define PIN_SPI_MOSI         (75u)
#define PIN_SPI_MISO         (74u)
#define PIN_SPI_SCK          (76u)
#define BOARD_SPI_SS0        (10u)
#define BOARD_SPI_SS1        (4u)
#define BOARD_SPI_SS2        (52u)
#define BOARD_SPI_SS3        PIN_SPI_SS3
#define BOARD_SPI_DEFAULT_SS BOARD_SPI_SS3

#define BOARD_PIN_TO_SPI_PIN(x) \
	(x==BOARD_SPI_SS0 ? PIN_SPI_SS0 : \
	(x==BOARD_SPI_SS1 ? PIN_SPI_SS1 : \
	(x==BOARD_SPI_SS2 ? PIN_SPI_SS2 : PIN_SPI_SS3 )))
#define BOARD_PIN_TO_SPI_CHANNEL(x) \
	(x==BOARD_SPI_SS0 ? 0 : \
	(x==BOARD_SPI_SS1 ? 1 : \
	(x==BOARD_SPI_SS2 ? 2 : 3)))

#define NUM_DIGITAL_PINS     (66u)
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

#define PINS_COUNT           (79u)
#define PINS_CAN0            (90u)
#define PINS_CAN1            (91u)
#define ADC_FREQ_MAX   20000000
#define ADC_STARTUP_FAST     12

const uint8_t SS   = BOARD_SPI_SS0;
const uint8_t SS1  = BOARD_SPI_SS1;
const uint8_t SS2  = BOARD_SPI_SS2;
const uint8_t SS3  = BOARD_SPI_SS3;
const uint8_t MOSI = PIN_SPI_MOSI;
const uint8_t MISO = PIN_SPI_MISO;
const uint8_t SCK  = PIN_SPI_SCK;

/*
 * DUE Board pin   |  PORT  | Label
 * ----------------+--------+-------
 *   0             |  PA8   | "RX0"
 *   1             |  PA9   | "TX0"
 *   2       TIOA0 |  PB25  |
 *   3       TIOA7 |  PC28  |
 *   4       NPCS1 |  PA29  |
 *           TIOB6 |  PC26  |
 *   5       TIOA6 |  PC25  |
 *   6       PWML7 |  PC24  |
 *   7       PWML6 |  PC23  |
 *   8       PWML5 |  PC22  |
 *   9       PWML4 |  PC21  |
 *  10       NPCS0 |  PA28  |
 *           TIOB7 |  PC29  |
 *  11       TIOA8 |  PD7   |
 *  12       TIOB8 |  PD8   |
 *  13       TIOB0 |  PB27  | LED AMBER "L"
 *  14       TXD3  |  PD4   | "TX3"
 *  15       RXD3  |  PD5   | "RX3"
 *  16       TXD1  |  PA13  | "TX2"
 *  17       RXD1  |  PA12  | "RX2"
 *  18       TXD0  |  PA11  | "TX1"
 *  19       RXD0  |  PA10  | "RX1"
 *  20             |  PB12  | "SDA"
 *  21             |  PB13  | "SCL"
 *  22             |  PB26  |
 *  23             |  PA14  |
 *  24             |  PA15  |
 *  25             |  PD0   |
 *  26             |  PD1   |
 *  27             |  PD2   |
 *  28             |  PD3   |
 *  29             |  PD6   |
 *  30             |  PD9   |
 *  31             |  PA7   |
 *  32             |  PD10  |
 *  33             |  PC1   |
 *  34             |  PC2   |
 *  35             |  PC3   |
 *  36             |  PC4   |
 *  37             |  PC5   |
 *  38             |  PC6   |
 *  39             |  PC7   |
 *  40             |  PC8   |
 *  41             |  PC9   |
 *  42             |  PA19  |
 *  43             |  PA20  |
 *  44             |  PC19  |
 *  45             |  PC18  |
 *  46             |  PC17  |
 *  47             |  PC16  |
 *  48             |  PC15  |
 *  49             |  PC14  |
 *  50             |  PC13  |
 *  51             |  PC12  |
 *  52       NPCS2 |  PB21  |
 *  53             |  PB14  |
 *  54             |  PA16  | "A0"
 *  55             |  PA24  | "A1"
 *  56             |  PA23  | "A2"
 *  57             |  PA22  | "A3"
 *  58       TIOB2 |  PA6   | "A4"
 *  69             |  PA4   | "A5"
 *  60       TIOB1 |  PA3   | "A6"
 *  61       TIOA1 |  PA2   | "A7"
 *  62             |  PB17  | "A8"
 *  63             |  PB18  | "A9"
 *  64             |  PB19  | "A10"
 *  65             |  PB20  | "A11"
 *  66             |  PB15  | "DAC0"
 *  67             |  PB16  | "DAC1"
 *  68             |  PA1   | "CANRX"
 *  69             |  PA0   | "CANTX"
 *  70             |  PA17  | "SDA1"
 *  71             |  PA18  | "SCL1"
 *  72             |  PC30  | LED AMBER "RX"
 *  73             |  PA21  | LED AMBER "TX"
 *  74       MISO  |  PA25  |
 *  75       MOSI  |  PA26  |
 *  76       SCLK  |  PA27  |
 *  77       NPCS0 |  PA28  |
 *  78       NPCS3 |  PB23  | unconnected!
 *
 * USB pin         |  PORT
 * ----------------+--------
 *  ID             |  PB11
 *  VBOF           |  PB10
 *
 */
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
/*
 * Pins descriptions
 */
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

 #define PIO_DEFAULT (0u << 0)
 #define PIN_ATTR_COMBO         (1UL<<0)
 #define PIN_ATTR_ANALOG        (1UL<<1)
 #define PIN_ATTR_DIGITAL       (1UL<<2)
 #define PIN_ATTR_PWM           (1UL<<3)
 #define PIN_ATTR_TIMER         (1UL<<4)

/*extern*/ const PinDescription g_APinDescription[]=
{
  // 0 .. 53 - Digital pins
  // ----------------------
  // 0/1 - UART (Serial)
  { PIOA, PIO_PA8A_URXD,     ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // URXD
  { PIOA, PIO_PA9A_UTXD,     ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT,  PIN_ATTR_DIGITAL,                 NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // UTXD
  // 2
  { PIOB, PIO_PB25B_TIOA0,   ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC0_CHA0     }, // TIOA0
  { PIOC, PIO_PC28B_TIOA7,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHA7     }, // TIOA7
  { PIOC, PIO_PC26B_TIOB6,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHB6     }, // TIOB6
  // 5
  { PIOC, PIO_PC25B_TIOA6,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHA6     }, // TIOA6
  { PIOC, PIO_PC24B_PWML7,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH7,     NOT_ON_TIMER }, // PWML7
  { PIOC, PIO_PC23B_PWML6,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH6,     NOT_ON_TIMER }, // PWML6
  { PIOC, PIO_PC22B_PWML5,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH5,     NOT_ON_TIMER }, // PWML5
  { PIOC, PIO_PC21B_PWML4,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM),   NO_ADC, NO_ADC, PWM_CH4,     NOT_ON_TIMER }, // PWML4
  // 10
  { PIOC, PIO_PC29B_TIOB7,   ID_PIOC, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHB7     }, // TIOB7
  { PIOD, PIO_PD7B_TIOA8,    ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHA8     }, // TIOA8
  { PIOD, PIO_PD8B_TIOB8,    ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC2_CHB8     }, // TIOB8
  // 13 - AMBER LED
  { PIOB, PIO_PB27B_TIOB0,   ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_TIMER), NO_ADC, NO_ADC, NOT_ON_PWM,  TC0_CHB0     }, // TIOB0
  // 14/15 - USART3 (Serial3)
  { PIOD, PIO_PD4B_TXD3,     ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // TXD3
  { PIOD, PIO_PD5B_RXD3,     ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // RXD3
  // 16/17 - USART1 (Serial2)
  { PIOA, PIO_PA13A_TXD1,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // TXD1
  { PIOA, PIO_PA12A_RXD1,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // RXD1
  // 18/19 - USART0 (Serial1)
  { PIOA, PIO_PA11A_TXD0,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // TXD0
  { PIOA, PIO_PA10A_RXD0,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // RXD0
  // 20/21 - TWI1
  { PIOB, PIO_PB12A_TWD1,    ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // TWD1 - SDA0
  { PIOB, PIO_PB13A_TWCK1,   ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // TWCK1 - SCL0
  // 22
  { PIOB, PIO_PB26,          ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 22
  { PIOA, PIO_PA14,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 23
  { PIOA, PIO_PA15,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 24
  { PIOD, PIO_PD0,           ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 25
  // 26
  { PIOD, PIO_PD1,           ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 26
  { PIOD, PIO_PD2,           ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 27
  { PIOD, PIO_PD3,           ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 28
  { PIOD, PIO_PD6,           ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 29
  // 30
  { PIOD, PIO_PD9,           ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 30
  { PIOA, PIO_PA7,           ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 31
  { PIOD, PIO_PD10,          ID_PIOD, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 32
  { PIOC, PIO_PC1,           ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 33
  // 34
  { PIOC, PIO_PC2,           ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 34
  { PIOC, PIO_PC3,           ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 35
  { PIOC, PIO_PC4,           ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 36
  { PIOC, PIO_PC5,           ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 37
  // 38
  { PIOC, PIO_PC6,           ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 38
  { PIOC, PIO_PC7,           ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 39
  { PIOC, PIO_PC8,           ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 40
  { PIOC, PIO_PC9,           ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 41
  // 42
  { PIOA, PIO_PA19,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 42
  { PIOA, PIO_PA20,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 43
  { PIOC, PIO_PC19,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 44
  { PIOC, PIO_PC18,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 45
  // 46
  { PIOC, PIO_PC17,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 46
  { PIOC, PIO_PC16,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 47
  { PIOC, PIO_PC15,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 48
  { PIOC, PIO_PC14,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 49
  // 50
  { PIOC, PIO_PC13,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 50
  { PIOC, PIO_PC12,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 51
  { PIOB, PIO_PB21,          ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 52
  { PIOB, PIO_PB14,          ID_PIOB, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // PIN 53
  // 54 .. 65 - Analog pins
  // ----------------------
  { PIOA, PIO_PA16X1_AD7,    ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC0,   ADC7,   NOT_ON_PWM,  NOT_ON_TIMER }, // AD0
  { PIOA, PIO_PA24X1_AD6,    ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC1,   ADC6,   NOT_ON_PWM,  NOT_ON_TIMER }, // AD1
  { PIOA, PIO_PA23X1_AD5,    ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC2,   ADC5,   NOT_ON_PWM,  NOT_ON_TIMER }, // AD2
  { PIOA, PIO_PA22X1_AD4,    ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC3,   ADC4,   NOT_ON_PWM,  NOT_ON_TIMER }, // AD3
  // 58
  { PIOA, PIO_PA6X1_AD3,     ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC4,   ADC3,   NOT_ON_PWM,  TC0_CHB2     }, // AD4
  { PIOA, PIO_PA4X1_AD2,     ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC5,   ADC2,   NOT_ON_PWM,  NOT_ON_TIMER }, // AD5
  { PIOA, PIO_PA3X1_AD1,     ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC6,   ADC1,   NOT_ON_PWM,  TC0_CHB1     }, // AD6
  { PIOA, PIO_PA2X1_AD0,     ID_PIOA, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC7,   ADC0,   NOT_ON_PWM,  TC0_CHA1     }, // AD7
  // 62
  { PIOB, PIO_PB17X1_AD10,   ID_PIOB, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC8,   ADC10,  NOT_ON_PWM,  NOT_ON_TIMER }, // AD8
  { PIOB, PIO_PB18X1_AD11,   ID_PIOB, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC9,   ADC11,  NOT_ON_PWM,  NOT_ON_TIMER }, // AD9
  { PIOB, PIO_PB19X1_AD12,   ID_PIOB, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC10,  ADC12,  NOT_ON_PWM,  NOT_ON_TIMER }, // AD10
  { PIOB, PIO_PB20X1_AD13,   ID_PIOB, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC11,  ADC13,  NOT_ON_PWM,  NOT_ON_TIMER }, // AD11
  // 66/67 - DAC0/DAC1
  { PIOB, PIO_PB15X1_DAC0,   ID_PIOB, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC12,  DA0,    NOT_ON_PWM,  NOT_ON_TIMER }, // DAC0
  { PIOB, PIO_PB16X1_DAC1,   ID_PIOB, PIO_INPUT,    PIO_DEFAULT, PIN_ATTR_ANALOG,                   ADC13,  DA1,    NOT_ON_PWM,  NOT_ON_TIMER }, // DAC1
  // 68/69 - CANRX0/CANTX0
  { PIOA, PIO_PA1A_CANRX0,   ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  ADC14,  NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // CANRX
  { PIOA, PIO_PA0A_CANTX0,   ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  ADC15,  NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // CANTX
  // 70/71 - TWI0
  { PIOA, PIO_PA17A_TWD0,    ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // TWD0 - SDA1
  { PIOA, PIO_PA18A_TWCK0,   ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // TWCK0 - SCL1
  // 72/73 - LEDs
  { PIOC, PIO_PC30,          ID_PIOC, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // LED AMBER RXL
  { PIOA, PIO_PA21,          ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // LED AMBER TXL
  // 74/75/76 - SPI
  { PIOA, PIO_PA25A_SPI0_MISO,ID_PIOA,PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // MISO
  { PIOA, PIO_PA26A_SPI0_MOSI,ID_PIOA,PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // MOSI
  { PIOA, PIO_PA27A_SPI0_SPCK,ID_PIOA,PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // SPCK
  // 77 - SPI CS0
  { PIOA, PIO_PA28A_SPI0_NPCS0,ID_PIOA,PIO_PERIPH_A,PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // NPCS0
  // 78 - SPI CS3 (unconnected)
  { PIOB, PIO_PB23B_SPI0_NPCS3,ID_PIOB,PIO_PERIPH_B,PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // NPCS3
  // 79 .. 84 - "All pins" masks
  // 79 - TWI0 all pins
  { PIOA, PIO_PA17A_TWD0|PIO_PA18A_TWCK0, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NOT_ON_PWM, NOT_ON_TIMER },
  // 80 - TWI1 all pins
  { PIOB, PIO_PB12A_TWD1|PIO_PB13A_TWCK1, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NOT_ON_PWM, NOT_ON_TIMER },
  // 81 - UART (Serial) all pins
  { PIOA, PIO_PA8A_URXD|PIO_PA9A_UTXD, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NOT_ON_PWM, NOT_ON_TIMER },
  // 82 - USART0 (Serial1) all pins
  { PIOA, PIO_PA11A_TXD0|PIO_PA10A_RXD0, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NOT_ON_PWM, NOT_ON_TIMER },
  // 83 - USART1 (Serial2) all pins
  { PIOA, PIO_PA13A_TXD1|PIO_PA12A_RXD1, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NOT_ON_PWM, NOT_ON_TIMER },
  // 84 - USART3 (Serial3) all pins
  { PIOD, PIO_PD4B_TXD3|PIO_PD5B_RXD3, ID_PIOD, PIO_PERIPH_B, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NOT_ON_PWM, NOT_ON_TIMER },
  // 85 - USB
  { PIOB, PIO_PB11A_UOTGID|PIO_PB10A_UOTGVBOF, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // ID - VBOF
  // 86 - SPI CS2
  { PIOB, PIO_PB21B_SPI0_NPCS2, ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // NPCS2
  // 87 - SPI CS1
  { PIOA, PIO_PA29A_SPI0_NPCS1, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // NPCS1
  // 88/89 - CANRX1/CANTX1 (same physical pin for 66/53)
  { PIOB, PIO_PB15A_CANRX1,     ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // CANRX1
  { PIOB, PIO_PB14A_CANTX1,     ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, PIN_ATTR_DIGITAL,                  NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER }, // CANTX1
  // 90 .. 91 - "All CAN pins" masks
  // 90 - CAN0 all pins
  { PIOA, PIO_PA1A_CANRX0|PIO_PA0A_CANTX0, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC,  NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER },
  // 91 - CAN1 all pins
  { PIOB, PIO_PB15A_CANRX1|PIO_PB14A_CANTX1, ID_PIOB, PIO_PERIPH_A, PIO_DEFAULT, (PIN_ATTR_DIGITAL|PIN_ATTR_COMBO), NO_ADC, NO_ADC, NOT_ON_PWM,  NOT_ON_TIMER },
  // END
  { NULL, 0, 0, PIO_NOT_A_PIN, PIO_DEFAULT, 0, NO_ADC, NO_ADC, NOT_ON_PWM, NOT_ON_TIMER }
} ;

#endif /* _VARIANT_ARDUINO_DUE_X_ */
