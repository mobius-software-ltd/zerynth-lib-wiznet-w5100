//#pragma once
#ifndef _VARIANT_ARDUINO_DUE_X_
#define _VARIANT_ARDUINO_DUE_X_

#include "inttypes.h"
#include <stdio.h>
#include "system_Device.h"
#include "sam3x8e.h"

/*
 * SPI Interfaces
 */
# define MAX_PERIPH_ID 31
#define VARIANT_MCK			84000000

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
extern uint8_t g_pinStatus[];
#define PIN_STATUS_DIGITAL_INPUT_PULLUP  (0x01)
#define PIN_STATUS_DIGITAL_INPUT         (0x02)
#define PIN_STATUS_DIGITAL_OUTPUT        (0x03)
#define PIN_STATUS_ANALOG                (0x04)
#define PIN_STATUS_PWM                   (0x05)
#define PIN_STATUS_TIMER                 (0x06)
#define PIN_STATUS_SERIAL                (0x07)
#define PIN_STATUS_DW_LOW                (0x10)
#define PIN_STATUS_DW_HIGH               (0x11)
#define PIO_PULLUP  								(1u << 0)
#define PIO_OPENDRAIN               (1u << 2)

#define PINS_COUNT           (79u)
#define PINS_CAN0            (90u)
#define PINS_CAN1            (91u)
#define ADC_FREQ_MAX   20000000
#define ADC_STARTUP_FAST     12

extern const uint8_t SS;
extern const uint8_t SS1;
extern const  uint8_t SS2;
extern const uint8_t SS3;
extern const uint8_t MOSI;
extern const uint8_t MISO;
extern const uint8_t SCK;

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

#define   ADC_MR_TRGSEL_ADC_TRIG5 (0x5u << 1)
#define   ADC_MR_TRGSEL_ADC_TRIG4 (0x4u << 1)
#define   ADC_MR_TRGSEL_ADC_TRIG3 (0x3u << 1)
#define   ADC_MR_TRGSEL_ADC_TRIG2 (0x2u << 1)
#define   ADC_MR_TRGSEL_ADC_TRIG1 (0x1u << 1)
#define ADC_MR_TRGSEL_Pos 1
#define ADC_MR_TRGSEL_Msk (0x7u << ADC_MR_TRGSEL_Pos)
#define   ADC_MR_TRGSEL_ADC_TRIG0 (0x0u << 1)
#define   ADC_MR_TRGEN_DIS (0x0u << 0)
#define ADC_MR_TRGEN (0x1u << 0)

 enum adc_trigger_t {
 	ADC_TRIG_SW = ADC_MR_TRGEN_DIS,  /* Starting a conversion is only possible by software. */
 	ADC_TRIG_EXT = ((ADC_MR_TRGSEL_ADC_TRIG0 << ADC_MR_TRGSEL_Pos) &
 									ADC_MR_TRGSEL_Msk) | ADC_MR_TRGEN,  /* External trigger */
 	ADC_TRIG_TIO_CH_0 = (ADC_MR_TRGSEL_ADC_TRIG1  & ADC_MR_TRGSEL_Msk) |
 											ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 0 */
 	ADC_TRIG_TIO_CH_1 = (ADC_MR_TRGSEL_ADC_TRIG2  & ADC_MR_TRGSEL_Msk) |
 											ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 1 */
 	ADC_TRIG_TIO_CH_2 = (ADC_MR_TRGSEL_ADC_TRIG3  & ADC_MR_TRGSEL_Msk) |
 											ADC_MR_TRGEN,  /* TIO Output of the Timer Counter Channel 2 */
 	ADC_TRIG_PWM_EVENT_LINE_0 = (ADC_MR_TRGSEL_ADC_TRIG4  & ADC_MR_TRGSEL_Msk) |
 															ADC_MR_TRGEN, /* PWM Event Line 0 */
 	ADC_TRIG_PWM_EVENT_LINE_1 = (ADC_MR_TRGSEL_ADC_TRIG5  & ADC_MR_TRGSEL_Msk) |
 															ADC_MR_TRGEN  /* PWM Event Line 1 */
 } ;

#define   ADC_MR_SETTLING_AST3 (0x0u << 20)
#define   ADC_MR_SETTLING_AST5 (0x1u << 20)
#define   ADC_MR_SETTLING_AST9 (0x2u << 20)
#define   ADC_MR_SETTLING_AST17 (0x3u << 20)

 enum adc_settling_time_t{
 	ADC_SETTLING_TIME_0 = ADC_MR_SETTLING_AST3,
 	ADC_SETTLING_TIME_1 = ADC_MR_SETTLING_AST5,
 	ADC_SETTLING_TIME_2 = ADC_MR_SETTLING_AST9,
 	ADC_SETTLING_TIME_3 = ADC_MR_SETTLING_AST17
 };

#define   ADC_MR_TRGEN_EN 	   (0x1u << 0)
#define ADC_MR_FREERUN_ON 		 (0x1u << 7)
#define ADC_MR_FREERUN 				 (0x1u << 7)
#define ADC_MR_TRGSEL_Pos 			1
#define ADC_MR_TRGSEL_Msk 		 (0x7u << ADC_MR_TRGSEL_Pos)
#define ADC_MR_TRGEN 					 (0x1u << 0)
#define ADC_MR_STARTUP_Pos 			16
#define ADC_MR_STARTUP_Msk 		 (0xfu << ADC_MR_STARTUP_Pos)
#define ADC_PTCR_TXTDIS 			 (0x1u << 9)
#define ADC_PTCR_RXTDIS 			 (0x1u << 1)
#define ADC_CR_SWRST 					 (0x1u << 0)
#define ADC_MR_TRANSFER_Pos 		28
#define ADC_MR_TRANSFER_Msk 		(0x3u << ADC_MR_TRANSFER_Pos)
#define ADC_MR_TRANSFER(value) ((ADC_MR_TRANSFER_Msk & ((value) << ADC_MR_TRANSFER_Pos)))
#define ADC_MR_TRACKTIM_Pos 		24
#define ADC_MR_TRACKTIM_Msk 		(0xfu << ADC_MR_TRACKTIM_Pos) /**< \brief (ADC_MR) Tracking Time */
#define ADC_MR_TRACKTIM(value) ((ADC_MR_TRACKTIM_Msk & ((value) << ADC_MR_TRACKTIM_Pos)))
#define ADC_MR_PRESCAL_Pos 8
#define ADC_MR_PRESCAL_Msk (0xffu << ADC_MR_PRESCAL_Pos) /**< \brief (ADC_MR) Prescaler Rate Selection */
#define ADC_MR_PRESCAL(value) ((ADC_MR_PRESCAL_Msk & ((value) << ADC_MR_PRESCAL_Pos)))


#define PIO_DEBOUNCE           (1u << 3)
#define PIO_DEGLITCH           (1u << 1)
#define PIO_DEFAULT 					 (0u << 0)
#define PIN_ATTR_COMBO         (1UL<<0)
#define PIN_ATTR_ANALOG        (1UL<<1)
#define PIN_ATTR_DIGITAL       (1UL<<2)
#define PIN_ATTR_PWM           (1UL<<3)
#define PIN_ATTR_TIMER         (1UL<<4)

#define SysTick_CTRL_ENABLE_Pos             0
#define SysTick_CTRL_ENABLE_Msk            (1UL << SysTick_CTRL_ENABLE_Pos)
#define SysTick_CTRL_TICKINT_Pos            1
#define SysTick_CTRL_TICKINT_Msk           (1UL << SysTick_CTRL_TICKINT_Pos)
#define SysTick_CTRL_CLKSOURCE_Pos          2
#define SysTick_CTRL_CLKSOURCE_Msk         (1UL << SysTick_CTRL_CLKSOURCE_Pos)
#define SysTick_LOAD_RELOAD_Pos             0
#define SysTick_LOAD_RELOAD_Msk  					(0xFFFFFFUL << SysTick_LOAD_RELOAD_Pos)

extern const PinDescription g_APinDescription[];

uint32_t PIO_Configure( Pio* pPio, const EPioType dwType, const uint32_t dwMask, const uint32_t dwAttribute );
uint32_t pmc_enable_periph_clk(uint32_t ul_id);
uint32_t pmc_disable_periph_clk(uint32_t ul_id);
void digitalWrite( uint32_t dwPin, uint32_t dwVal );

void PIO_DisableInterrupt( Pio* pPio, const uint32_t dwMask ) ;
void PIO_PullUp( Pio* pPio, const uint32_t dwMask, const uint32_t dwPullUpEnable ) ;
void PIO_SetInput( Pio* pPio, uint32_t dwMask, uint32_t dwAttribute ) ;
void PIO_SetOutput( Pio* pPio, uint32_t dwMask, uint32_t dwDefaultValue,uint32_t dwMultiDriveEnable, uint32_t dwPullUpEnable ) ;
uint32_t PIO_GetOutputDataStatus( const Pio* pPio, const uint32_t dwMask ) ;

uint32_t adc_init(Adc *p_adc, const uint32_t ul_mck, const uint32_t ul_adc_clock, const uint8_t uc_startup);
void adc_configure_timing(Adc *p_adc, const uint8_t uc_tracking,const enum adc_settling_time_t settling, const uint8_t uc_transfer);
void adc_configure_trigger(Adc *p_adc, const enum adc_trigger_t trigger,const uint8_t uc_freerun);
void adc_disable_interrupt(Adc *p_adc, const uint32_t ul_source);
void adc_disable_all_channel(Adc *p_adc);
uint32_t SysTick_Config(uint32_t ticks);

#define     __IO    volatile
#define 		__I 		volatile const

typedef struct
{
  __IO uint32_t CTRL;                    /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
  __IO uint32_t LOAD;                    /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register       */
  __IO uint32_t VAL;                     /*!< Offset: 0x008 (R/W)  SysTick Current Value Register      */
  __I  uint32_t CALIB;                   /*!< Offset: 0x00C (R/ )  SysTick Calibration Register        */
} SysTick_Type;

typedef struct
{
  __I  uint32_t CPUID;                   /*!< Offset: 0x000 (R/ )  CPUID Base Register                                   */
  __IO uint32_t ICSR;                    /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register                  */
       uint32_t RESERVED0;
  __IO uint32_t AIRCR;                   /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register      */
  __IO uint32_t SCR;                     /*!< Offset: 0x010 (R/W)  System Control Register                               */
  __IO uint32_t CCR;                     /*!< Offset: 0x014 (R/W)  Configuration Control Register                        */
       uint32_t RESERVED1;
  __IO uint32_t SHP[2];                  /*!< Offset: 0x01C (R/W)  System Handlers Priority Registers. [0] is RESERVED   */
  __IO uint32_t SHCSR;                   /*!< Offset: 0x024 (R/W)  System Handler Control and State Register             */
} SCB_Type;
#define SCB_SHCSR_SYSTICKACT_Pos           11
#define SCB_SHCSR_SYSTICKACT_Msk           (1UL << SCB_SHCSR_SYSTICKACT_Pos)

#define SCB_ICSR_PENDSTSET_Pos             26
#define SCB_ICSR_PENDSTSET_Msk             (1UL << SCB_ICSR_PENDSTSET_Pos)
/* SysTick Control / Status Register Definitions */
#define SysTick_CTRL_COUNTFLAG_Pos         16                                             /*!< SysTick CTRL: COUNTFLAG Position */
#define SysTick_CTRL_COUNTFLAG_Msk         (1UL << SysTick_CTRL_COUNTFLAG_Pos)            /*!< SysTick CTRL: COUNTFLAG Mask */

#define SysTick_CTRL_CLKSOURCE_Pos          2                                             /*!< SysTick CTRL: CLKSOURCE Position */
#define SysTick_CTRL_CLKSOURCE_Msk         (1UL << SysTick_CTRL_CLKSOURCE_Pos)            /*!< SysTick CTRL: CLKSOURCE Mask */

#define SysTick_CTRL_TICKINT_Pos            1                                             /*!< SysTick CTRL: TICKINT Position */
#define SysTick_CTRL_TICKINT_Msk           (1UL << SysTick_CTRL_TICKINT_Pos)              /*!< SysTick CTRL: TICKINT Mask */

#define SysTick_CTRL_ENABLE_Pos             0                                             /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk            (1UL << SysTick_CTRL_ENABLE_Pos)               /*!< SysTick CTRL: ENABLE Mask */

/* SysTick Reload Register Definitions */
#define SysTick_LOAD_RELOAD_Pos             0                                             /*!< SysTick LOAD: RELOAD Position */
#define SysTick_LOAD_RELOAD_Msk            (0xFFFFFFUL << SysTick_LOAD_RELOAD_Pos)        /*!< SysTick LOAD: RELOAD Mask */

/* SysTick Current Register Definitions */
#define SysTick_VAL_CURRENT_Pos             0                                             /*!< SysTick VAL: CURRENT Position */
#define SysTick_VAL_CURRENT_Msk            (0xFFFFFFUL << SysTick_VAL_CURRENT_Pos)        /*!< SysTick VAL: CURRENT Mask */

/* SysTick Calibration Register Definitions */
#define SysTick_CALIB_NOREF_Pos            31                                             /*!< SysTick CALIB: NOREF Position */
#define SysTick_CALIB_NOREF_Msk            (1UL << SysTick_CALIB_NOREF_Pos)               /*!< SysTick CALIB: NOREF Mask */

#define SysTick_CALIB_SKEW_Pos             30                                             /*!< SysTick CALIB: SKEW Position */
#define SysTick_CALIB_SKEW_Msk             (1UL << SysTick_CALIB_SKEW_Pos)                /*!< SysTick CALIB: SKEW Mask */

#define SysTick_CALIB_TENMS_Pos             0                                             /*!< SysTick CALIB: TENMS Position */
#define SysTick_CALIB_TENMS_Msk            (0xFFFFFFUL << SysTick_VAL_CURRENT_Pos)        /*!< SysTick CALIB: TENMS Mask */

/* Memory mapping of Cortex-M0 Hardware */
#define SCS_BASE            (0xE000E000UL)                            /*!< System Control Space Base Address */
//#define CoreDebug_BASE      (0xE000EDF0UL)                            /*!< Core Debug Base Address           */
#define SysTick_BASE        (SCS_BASE +  0x0010UL)                    /*!< SysTick Base Address              */
//#define NVIC_BASE           (SCS_BASE +  0x0100UL)                    /*!< NVIC Base Address                 */
#define SCB_BASE            (SCS_BASE +  0x0D00UL)                    /*!< System Control Block Base Address */
#define SCB                 ((SCB_Type       *)     SCB_BASE      )   /*!< SCB configuration struct           */
#define SysTick             ((SysTick_Type   *)     SysTick_BASE  )   /*!< SysTick configuration struct       */
//#define NVIC                ((NVIC_Type      *)     NVIC_BASE     )   /*!< NVIC configuration struct          */


void analogOutputInit( void ) ;

#endif /* _VARIANT_ARDUINO_DUE_X_ */
