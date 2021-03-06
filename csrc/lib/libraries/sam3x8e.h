//#pragma once

#ifndef _SAM3X8E_
#define _SAM3X8E_

#ifndef _SAM3N_PIO_COMPONENT_
#include "component_pio.h"
#endif

/* ************************************************************************** */
/*   CMSIS DEFINITIONS FOR SAM3X8E */
/* ************************************************************************** */
/** \addtogroup SAM3X8E_cmsis CMSIS Definitions */
/*@{*/

/**< Interrupt Number Definition */
typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ******************************/
  NonMaskableInt_IRQn   = -14, /**<  2 Non Maskable Interrupt                */
  MemoryManagement_IRQn = -12, /**<  4 Cortex-M3 Memory Management Interrupt */
  BusFault_IRQn         = -11, /**<  5 Cortex-M3 Bus Fault Interrupt         */
  UsageFault_IRQn       = -10, /**<  6 Cortex-M3 Usage Fault Interrupt       */
  SVCall_IRQn           = -5,  /**< 11 Cortex-M3 SV Call Interrupt           */
  DebugMonitor_IRQn     = -4,  /**< 12 Cortex-M3 Debug Monitor Interrupt     */
  PendSV_IRQn           = -2,  /**< 14 Cortex-M3 Pend SV Interrupt           */
  SysTick_IRQn          = -1,  /**< 15 Cortex-M3 System Tick Interrupt       */
/******  SAM3X8E specific Interrupt Numbers *********************************/

  SUPC_IRQn            =  0, /**<  0 SAM3X8E Supply Controller (SUPC) */
  RSTC_IRQn            =  1, /**<  1 SAM3X8E Reset Controller (RSTC) */
  RTC_IRQn             =  2, /**<  2 SAM3X8E Real Time Clock (RTC) */
  RTT_IRQn             =  3, /**<  3 SAM3X8E Real Time Timer (RTT) */
  WDT_IRQn             =  4, /**<  4 SAM3X8E Watchdog Timer (WDT) */
  PMC_IRQn             =  5, /**<  5 SAM3X8E Power Management Controller (PMC) */
  EFC0_IRQn            =  6, /**<  6 SAM3X8E Enhanced Flash Controller 0 (EFC0) */
  EFC1_IRQn            =  7, /**<  7 SAM3X8E Enhanced Flash Controller 1 (EFC1) */
  UART_IRQn            =  8, /**<  8 SAM3X8E Universal Asynchronous Receiver Transceiver (UART) */
  SMC_IRQn             =  9, /**<  9 SAM3X8E Static Memory Controller (SMC) */
  PIOA_IRQn            = 11, /**< 11 SAM3X8E Parallel I/O Controller A, (PIOA) */
  PIOB_IRQn            = 12, /**< 12 SAM3X8E Parallel I/O Controller B (PIOB) */
  PIOC_IRQn            = 13, /**< 13 SAM3X8E Parallel I/O Controller C (PIOC) */
  PIOD_IRQn            = 14, /**< 14 SAM3X8E Parallel I/O Controller D (PIOD) */
  USART0_IRQn          = 17, /**< 17 SAM3X8E USART 0 (USART0) */
  USART1_IRQn          = 18, /**< 18 SAM3X8E USART 1 (USART1) */
  USART2_IRQn          = 19, /**< 19 SAM3X8E USART 2 (USART2) */
  USART3_IRQn          = 20, /**< 20 SAM3X8E USART 3 (USART3) */
  HSMCI_IRQn           = 21, /**< 21 SAM3X8E Multimedia Card Interface (HSMCI) */
  TWI0_IRQn            = 22, /**< 22 SAM3X8E Two-Wire Interface 0 (TWI0) */
  TWI1_IRQn            = 23, /**< 23 SAM3X8E Two-Wire Interface 1 (TWI1) */
  SPI0_IRQn            = 24, /**< 24 SAM3X8E Serial Peripheral Interface (SPI0) */
  SSC_IRQn             = 26, /**< 26 SAM3X8E Synchronous Serial Controller (SSC) */
  TC0_IRQn             = 27, /**< 27 SAM3X8E Timer Counter 0 (TC0) */
  TC1_IRQn             = 28, /**< 28 SAM3X8E Timer Counter 1 (TC1) */
  TC2_IRQn             = 29, /**< 29 SAM3X8E Timer Counter 2 (TC2) */
  TC3_IRQn             = 30, /**< 30 SAM3X8E Timer Counter 3 (TC3) */
  TC4_IRQn             = 31, /**< 31 SAM3X8E Timer Counter 4 (TC4) */
  TC5_IRQn             = 32, /**< 32 SAM3X8E Timer Counter 5 (TC5) */
  TC6_IRQn             = 33, /**< 33 SAM3X8E Timer Counter 6 (TC6) */
  TC7_IRQn             = 34, /**< 34 SAM3X8E Timer Counter 7 (TC7) */
  TC8_IRQn             = 35, /**< 35 SAM3X8E Timer Counter 8 (TC8) */
  PWM_IRQn             = 36, /**< 36 SAM3X8E Pulse Width Modulation Controller (PWM) */
  ADC_IRQn             = 37, /**< 37 SAM3X8E ADC Controller (ADC) */
  DACC_IRQn            = 38, /**< 38 SAM3X8E DAC Controller (DACC) */
  DMAC_IRQn            = 39, /**< 39 SAM3X8E DMA Controller (DMAC) */
  UOTGHS_IRQn          = 40, /**< 40 SAM3X8E USB OTG High Speed (UOTGHS) */
  TRNG_IRQn            = 41, /**< 41 SAM3X8E True Random Number Generator (TRNG) */
  EMAC_IRQn            = 42, /**< 42 SAM3X8E Ethernet MAC (EMAC) */
  CAN0_IRQn            = 43, /**< 43 SAM3X8E CAN Controller 0 (CAN0) */
  CAN1_IRQn            = 44, /**< 44 SAM3X8E CAN Controller 1 (CAN1) */

  PERIPH_COUNT_IRQn    = 45  /**< Number of peripheral IDs */
} IRQn_Type;

typedef struct _DeviceVectors
{
  /* Stack pointer */
  void* pvStack;

  /* Cortex-M handlers */
  void* pfnReset_Handler;
  void* pfnNMI_Handler;
  void* pfnHardFault_Handler;
  void* pfnMemManage_Handler;
  void* pfnBusFault_Handler;
  void* pfnUsageFault_Handler;
  void* pfnReserved1_Handler;
  void* pfnReserved2_Handler;
  void* pfnReserved3_Handler;
  void* pfnReserved4_Handler;
  void* pfnSVC_Handler;
  void* pfnDebugMon_Handler;
  void* pfnReserved5_Handler;
  void* pfnPendSV_Handler;
  void* pfnSysTick_Handler;

  /* Peripheral handlers */
  void* pfnSUPC_Handler;   /*  0 Supply Controller */
  void* pfnRSTC_Handler;   /*  1 Reset Controller */
  void* pfnRTC_Handler;    /*  2 Real Time Clock */
  void* pfnRTT_Handler;    /*  3 Real Time Timer */
  void* pfnWDT_Handler;    /*  4 Watchdog Timer */
  void* pfnPMC_Handler;    /*  5 Power Management Controller */
  void* pfnEFC0_Handler;   /*  6 Enhanced Flash Controller 0 */
  void* pfnEFC1_Handler;   /*  7 Enhanced Flash Controller 1 */
  void* pfnUART_Handler;   /*  8 Universal Asynchronous Receiver Transceiver */
  void* pfnSMC_Handler;    /*  9 Static Memory Controller */
  void* pvReserved10;
  void* pfnPIOA_Handler;   /* 11 Parallel I/O Controller A, */
  void* pfnPIOB_Handler;   /* 12 Parallel I/O Controller B */
  void* pfnPIOC_Handler;   /* 13 Parallel I/O Controller C */
  void* pfnPIOD_Handler;   /* 14 Parallel I/O Controller D */
  void* pvReserved15;
  void* pvReserved16;
  void* pfnUSART0_Handler; /* 17 USART 0 */
  void* pfnUSART1_Handler; /* 18 USART 1 */
  void* pfnUSART2_Handler; /* 19 USART 2 */
  void* pfnUSART3_Handler; /* 20 USART 3 */
  void* pfnHSMCI_Handler;  /* 21 Multimedia Card Interface */
  void* pfnTWI0_Handler;   /* 22 Two-Wire Interface 0 */
  void* pfnTWI1_Handler;   /* 23 Two-Wire Interface 1 */
  void* pfnSPI0_Handler;   /* 24 Serial Peripheral Interface */
  void* pvReserved25;
  void* pfnSSC_Handler;    /* 26 Synchronous Serial Controller */
  void* pfnTC0_Handler;    /* 27 Timer Counter 0 */
  void* pfnTC1_Handler;    /* 28 Timer Counter 1 */
  void* pfnTC2_Handler;    /* 29 Timer Counter 2 */
  void* pfnTC3_Handler;    /* 30 Timer Counter 3 */
  void* pfnTC4_Handler;    /* 31 Timer Counter 4 */
  void* pfnTC5_Handler;    /* 32 Timer Counter 5 */
  void* pfnTC6_Handler;    /* 33 Timer Counter 6 */
  void* pfnTC7_Handler;    /* 34 Timer Counter 7 */
  void* pfnTC8_Handler;    /* 35 Timer Counter 8 */
  void* pfnPWM_Handler;    /* 36 Pulse Width Modulation Controller */
  void* pfnADC_Handler;    /* 37 ADC Controller */
  void* pfnDACC_Handler;   /* 38 DAC Controller */
  void* pfnDMAC_Handler;   /* 39 DMA Controller */
  void* pfnUOTGHS_Handler; /* 40 USB OTG High Speed */
  void* pfnTRNG_Handler;   /* 41 True Random Number Generator */
  void* pfnEMAC_Handler;   /* 42 Ethernet MAC */
  void* pfnCAN0_Handler;   /* 43 CAN Controller 0 */
  void* pfnCAN1_Handler;   /* 44 CAN Controller 1 */
} DeviceVectors;

typedef struct {
  WoReg ADC_CR;        /**< \brief (Adc Offset: 0x00) Control Register */
  RwReg ADC_MR;        /**< \brief (Adc Offset: 0x04) Mode Register */
  RwReg ADC_SEQR1;     /**< \brief (Adc Offset: 0x08) Channel Sequence Register 1 */
  RwReg ADC_SEQR2;     /**< \brief (Adc Offset: 0x0C) Channel Sequence Register 2 */
  WoReg ADC_CHER;      /**< \brief (Adc Offset: 0x10) Channel Enable Register */
  WoReg ADC_CHDR;      /**< \brief (Adc Offset: 0x14) Channel Disable Register */
  RoReg ADC_CHSR;      /**< \brief (Adc Offset: 0x18) Channel Status Register */
  RoReg Reserved1[1];
  RoReg ADC_LCDR;      /**< \brief (Adc Offset: 0x20) Last Converted Data Register */
  WoReg ADC_IER;       /**< \brief (Adc Offset: 0x24) Interrupt Enable Register */
  WoReg ADC_IDR;       /**< \brief (Adc Offset: 0x28) Interrupt Disable Register */
  RoReg ADC_IMR;       /**< \brief (Adc Offset: 0x2C) Interrupt Mask Register */
  RoReg ADC_ISR;       /**< \brief (Adc Offset: 0x30) Interrupt Status Register */
  RoReg Reserved2[2];
  RoReg ADC_OVER;      /**< \brief (Adc Offset: 0x3C) Overrun Status Register */
  RwReg ADC_EMR;       /**< \brief (Adc Offset: 0x40) Extended Mode Register */
  RwReg ADC_CWR;       /**< \brief (Adc Offset: 0x44) Compare Window Register */
  RwReg ADC_CGR;       /**< \brief (Adc Offset: 0x48) Channel Gain Register */
  RwReg ADC_COR;       /**< \brief (Adc Offset: 0x4C) Channel Offset Register */
  RoReg ADC_CDR[16];   /**< \brief (Adc Offset: 0x50) Channel Data Register */
  RoReg Reserved3[1];
  RwReg ADC_ACR;       /**< \brief (Adc Offset: 0x94) Analog Control Register */
  RoReg Reserved4[19];
  RwReg ADC_WPMR;      /**< \brief (Adc Offset: 0xE4) Write Protect Mode Register */
  RoReg ADC_WPSR;      /**< \brief (Adc Offset: 0xE8) Write Protect Status Register */
  RoReg Reserved5[5];
  RwReg ADC_RPR;       /**< \brief (Adc Offset: 0x100) Receive Pointer Register */
  RwReg ADC_RCR;       /**< \brief (Adc Offset: 0x104) Receive Counter Register */
  RoReg Reserved6[2];
  RwReg ADC_RNPR;      /**< \brief (Adc Offset: 0x110) Receive Next Pointer Register */
  RwReg ADC_RNCR;      /**< \brief (Adc Offset: 0x114) Receive Next Counter Register */
  RoReg Reserved7[2];
  WoReg ADC_PTCR;      /**< \brief (Adc Offset: 0x120) Transfer Control Register */
  RoReg ADC_PTSR;      /**< \brief (Adc Offset: 0x124) Transfer Status Register */
} Adc;

typedef struct {
  WoReg PMC_SCER;      /**< \brief (Pmc Offset: 0x0000) System Clock Enable Register */
  WoReg PMC_SCDR;      /**< \brief (Pmc Offset: 0x0004) System Clock Disable Register */
  RoReg PMC_SCSR;      /**< \brief (Pmc Offset: 0x0008) System Clock Status Register */
  RoReg Reserved1[1];
  WoReg PMC_PCER0;     /**< \brief (Pmc Offset: 0x0010) Peripheral Clock Enable Register 0 */
  WoReg PMC_PCDR0;     /**< \brief (Pmc Offset: 0x0014) Peripheral Clock Disable Register 0 */
  RoReg PMC_PCSR0;     /**< \brief (Pmc Offset: 0x0018) Peripheral Clock Status Register 0 */
  RwReg CKGR_UCKR;     /**< \brief (Pmc Offset: 0x001C) UTMI Clock Register */
  RwReg CKGR_MOR;      /**< \brief (Pmc Offset: 0x0020) Main Oscillator Register */
  RoReg CKGR_MCFR;     /**< \brief (Pmc Offset: 0x0024) Main Clock Frequency Register */
  RwReg CKGR_PLLAR;    /**< \brief (Pmc Offset: 0x0028) PLLA Register */
  RoReg Reserved2[1];
  RwReg PMC_MCKR;      /**< \brief (Pmc Offset: 0x0030) Master Clock Register */
  RoReg Reserved3[1];
  RwReg PMC_USB;       /**< \brief (Pmc Offset: 0x0038) USB Clock Register */
  RoReg Reserved4[1];
  RwReg PMC_PCK[3];    /**< \brief (Pmc Offset: 0x0040) Programmable Clock 0 Register */
  RoReg Reserved5[5];
  WoReg PMC_IER;       /**< \brief (Pmc Offset: 0x0060) Interrupt Enable Register */
  WoReg PMC_IDR;       /**< \brief (Pmc Offset: 0x0064) Interrupt Disable Register */
  RoReg PMC_SR;        /**< \brief (Pmc Offset: 0x0068) Status Register */
  RoReg PMC_IMR;       /**< \brief (Pmc Offset: 0x006C) Interrupt Mask Register */
  RwReg PMC_FSMR;      /**< \brief (Pmc Offset: 0x0070) Fast Startup Mode Register */
  RwReg PMC_FSPR;      /**< \brief (Pmc Offset: 0x0074) Fast Startup Polarity Register */
  WoReg PMC_FOCR;      /**< \brief (Pmc Offset: 0x0078) Fault Output Clear Register */
  RoReg Reserved6[26];
  RwReg PMC_WPMR;      /**< \brief (Pmc Offset: 0x00E4) Write Protect Mode Register */
  RoReg PMC_WPSR;      /**< \brief (Pmc Offset: 0x00E8) Write Protect Status Register */
  RoReg Reserved7[5];
  WoReg PMC_PCER1;     /**< \brief (Pmc Offset: 0x0100) Peripheral Clock Enable Register 1 */
  WoReg PMC_PCDR1;     /**< \brief (Pmc Offset: 0x0104) Peripheral Clock Disable Register 1 */
  RoReg PMC_PCSR1;     /**< \brief (Pmc Offset: 0x0108) Peripheral Clock Status Register 1 */
  RwReg PMC_PCR;       /**< \brief (Pmc Offset: 0x010C) Peripheral Control Register */
} Pmc;

/* Cortex-M3 core handlers */
void Reset_Handler      ( void );
void NMI_Handler        ( void );
void HardFault_Handler  ( void );
void MemManage_Handler  ( void );
void BusFault_Handler   ( void );
void UsageFault_Handler ( void );
void SVC_Handler        ( void );
void DebugMon_Handler   ( void );
void PendSV_Handler     ( void );
void SysTick_Handler    ( void );

/* Peripherals handlers */
void ADC_Handler        ( void );
void CAN0_Handler       ( void );
void CAN1_Handler       ( void );
void DACC_Handler       ( void );
void DMAC_Handler       ( void );
void EFC0_Handler       ( void );
void EFC1_Handler       ( void );
void EMAC_Handler       ( void );
void HSMCI_Handler      ( void );
void PIOA_Handler       ( void );
void PIOB_Handler       ( void );
void PIOC_Handler       ( void );
void PIOD_Handler       ( void );
void PMC_Handler        ( void );
void PWM_Handler        ( void );
void RSTC_Handler       ( void );
void RTC_Handler        ( void );
void RTT_Handler        ( void );
void SMC_Handler        ( void );
void SPI0_Handler       ( void );
void SSC_Handler        ( void );
void SUPC_Handler       ( void );
void TC0_Handler        ( void );
void TC1_Handler        ( void );
void TC2_Handler        ( void );
void TC3_Handler        ( void );
void TC4_Handler        ( void );
void TC5_Handler        ( void );
void TC6_Handler        ( void );
void TC7_Handler        ( void );
void TC8_Handler        ( void );
void TRNG_Handler       ( void );
void TWI0_Handler       ( void );
void TWI1_Handler       ( void );
void UART_Handler       ( void );
void UOTGHS_Handler     ( void );
void USART0_Handler     ( void );
void USART1_Handler     ( void );
void USART2_Handler     ( void );
void USART3_Handler     ( void );
void WDT_Handler        ( void );

/**
 * \brief Configuration of the Cortex-M3 Processor and Core Peripherals
 */

#define __CM3_REV              0x0200 /**< SAM3X8E core revision number ([15:8] revision number, [7:0] patch number) */
#define __MPU_PRESENT          1      /**< SAM3X8E does provide a MPU */
#define __NVIC_PRIO_BITS       4      /**< SAM3X8E uses 4 Bits for the Priority Levels */
#define __Vendor_SysTickConfig 0      /**< Set to 1 if different SysTick Config is used */


/* ************************************************************************** */
/*   PERIPHERAL ID DEFINITIONS FOR SAM3X8E */
/* ************************************************************************** */
/** \addtogroup SAM3X8E_id Peripheral Ids Definitions */
/*@{*/

#define ID_SUPC   ( 0) /**< \brief Supply Controller (SUPC) */
#define ID_RSTC   ( 1) /**< \brief Reset Controller (RSTC) */
#define ID_RTC    ( 2) /**< \brief Real Time Clock (RTC) */
#define ID_RTT    ( 3) /**< \brief Real Time Timer (RTT) */
#define ID_WDT    ( 4) /**< \brief Watchdog Timer (WDT) */
#define ID_PMC    ( 5) /**< \brief Power Management Controller (PMC) */
#define ID_EFC0   ( 6) /**< \brief Enhanced Flash Controller 0 (EFC0) */
#define ID_EFC1   ( 7) /**< \brief Enhanced Flash Controller 1 (EFC1) */
//#define ID_UART   ( 8) /**< \brief Universal Asynchronous Receiver Transceiver (UART) */
#define ID_SMC    ( 9) /**< \brief Static Memory Controller (SMC) */
#define ID_PIOA   (11) /**< \brief Parallel I/O Controller A, (PIOA) */
#define ID_PIOB   (12) /**< \brief Parallel I/O Controller B (PIOB) */
#define ID_PIOC   (13) /**< \brief Parallel I/O Controller C (PIOC) */
#define ID_PIOD   (14) /**< \brief Parallel I/O Controller D (PIOD) */
//#define ID_USART0 (17) /**< \brief USART 0 (USART0) */
//#define ID_USART1 (18) /**< \brief USART 1 (USART1) */
//#define ID_USART2 (19) /**< \brief USART 2 (USART2) */
//#define ID_USART3 (20) /**< \brief USART 3 (USART3) */
#define ID_HSMCI  (21) /**< \brief Multimedia Card Interface (HSMCI) */
#define ID_TWI0   (22) /**< \brief Two-Wire Interface 0 (TWI0) */
#define ID_TWI1   (23) /**< \brief Two-Wire Interface 1 (TWI1) */
enum {ID_SPI0 = 24};
#define ID_SPI0   ID_SPI0 /**< \brief Serial Peripheral Interface (SPI0) */
#define ID_SSC    (26) /**< \brief Synchronous Serial Controller (SSC) */
#define ID_TC0    (27) /**< \brief Timer Counter 0 (TC0) */
#define ID_TC1    (28) /**< \brief Timer Counter 1 (TC1) */
#define ID_TC2    (29) /**< \brief Timer Counter 2 (TC2) */
#define ID_TC3    (30) /**< \brief Timer Counter 3 (TC3) */
#define ID_TC4    (31) /**< \brief Timer Counter 4 (TC4) */
#define ID_TC5    (32) /**< \brief Timer Counter 5 (TC5) */
#define ID_TC6    (33) /**< \brief Timer Counter 6 (TC6) */
#define ID_TC7    (34) /**< \brief Timer Counter 7 (TC7) */
#define ID_TC8    (35) /**< \brief Timer Counter 8 (TC8) */
#define ID_PWM    (36) /**< \brief Pulse Width Modulation Controller (PWM) */
#define ID_ADC    (37) /**< \brief ADC Controller (ADC) */
#define ID_DACC   (38) /**< \brief DAC Controller (DACC) */
#define ID_DMAC   (39) /**< \brief DMA Controller (DMAC) */
#define ID_UOTGHS (40) /**< \brief USB OTG High Speed (UOTGHS) */
#define ID_TRNG   (41) /**< \brief True Random Number Generator (TRNG) */
#define ID_EMAC   (42) /**< \brief Ethernet MAC (EMAC) */
#define ID_CAN0   (43) /**< \brief CAN Controller 0 (CAN0) */
#define ID_CAN1   (44) /**< \brief CAN Controller 1 (CAN1) */

#define ID_PERIPH_COUNT (45) /**< \brief Number of peripheral IDs */
/*@}*/

/* ************************************************************************** */
/*   BASE ADDRESS DEFINITIONS FOR SAM3X8E */
/* ************************************************************************** */
/** \addtogroup SAM3X8E_base Peripheral Base Address Definitions */
/*@{*/
#define HSMCI      ((Hsmci  *)0x40000000U) /**< \brief (HSMCI     ) Base Address */
#define SSC        ((Ssc    *)0x40004000U) /**< \brief (SSC       ) Base Address */
enum {SPI0 = 0x40008000U};
#define SPI0 SPI0
//#define SPI0       ((Spi    *)0x40008000U) /**< \brief (SPI0      ) Base Address */
#define TC0        ((Tc     *)0x40080000U) /**< \brief (TC0       ) Base Address */
#define TC1        ((Tc     *)0x40084000U) /**< \brief (TC1       ) Base Address */
#define TC2        ((Tc     *)0x40088000U) /**< \brief (TC2       ) Base Address */
#define TWI0       ((Twi    *)0x4008C000U) /**< \brief (TWI0      ) Base Address */
#define PDC_TWI0   ((Pdc    *)0x4008C100U) /**< \brief (PDC_TWI0  ) Base Address */
#define TWI1       ((Twi    *)0x40090000U) /**< \brief (TWI1      ) Base Address */
#define PDC_TWI1   ((Pdc    *)0x40090100U) /**< \brief (PDC_TWI1  ) Base Address */
#define PWM        ((Pwm    *)0x40094000U) /**< \brief (PWM       ) Base Address */
#define PDC_PWM    ((Pdc    *)0x40094100U) /**< \brief (PDC_PWM   ) Base Address */
//#define USART0     ((Usart  *)0x40098000U) /**< \brief (USART0    ) Base Address */
//#define PDC_USART0 ((Pdc    *)0x40098100U) /**< \brief (PDC_USART0) Base Address */
//#define USART1     ((Usart  *)0x4009C000U) /**< \brief (USART1    ) Base Address */
//#define PDC_USART1 ((Pdc    *)0x4009C100U) /**< \brief (PDC_USART1) Base Address */
//#define USART2     ((Usart  *)0x400A0000U) /**< \brief (USART2    ) Base Address */
//#define PDC_USART2 ((Pdc    *)0x400A0100U) /**< \brief (PDC_USART2) Base Address */
//#define USART3     ((Usart  *)0x400A4000U) /**< \brief (USART3    ) Base Address */
//#define PDC_USART3 ((Pdc    *)0x400A4100U) /**< \brief (PDC_USART3) Base Address */
#define UOTGHS     ((Uotghs *)0x400AC000U) /**< \brief (UOTGHS    ) Base Address */
#define EMAC       ((Emac   *)0x400B0000U) /**< \brief (EMAC      ) Base Address */
#define CAN0       ((Can    *)0x400B4000U) /**< \brief (CAN0      ) Base Address */
#define CAN1       ((Can    *)0x400B8000U) /**< \brief (CAN1      ) Base Address */
#define TRNG       ((Trng   *)0x400BC000U) /**< \brief (TRNG      ) Base Address */
#define ADC        ((Adc    *)0x400C0000U) /**< \brief (ADC       ) Base Address */
#define PDC_ADC    ((Pdc    *)0x400C0100U) /**< \brief (PDC_ADC   ) Base Address */
#define DMAC       ((Dmac   *)0x400C4000U) /**< \brief (DMAC      ) Base Address */
#define DACC       ((Dacc   *)0x400C8000U) /**< \brief (DACC      ) Base Address */
#define PDC_DACC   ((Pdc    *)0x400C8100U) /**< \brief (PDC_DACC  ) Base Address */
#define SMC        ((Smc    *)0x400E0000U) /**< \brief (SMC       ) Base Address */
#define MATRIX     ((Matrix *)0x400E0400U) /**< \brief (MATRIX    ) Base Address */
#define PMC        ((Pmc    *)0x400E0600U) /**< \brief (PMC       ) Base Address */
//#define UART       ((Uart   *)0x400E0800U) /**< \brief (UART      ) Base Address */
#define PDC_UART   ((Pdc    *)0x400E0900U) /**< \brief (PDC_UART  ) Base Address */
#define CHIPID     ((Chipid *)0x400E0940U) /**< \brief (CHIPID    ) Base Address */
#define EFC0       ((Efc    *)0x400E0A00U) /**< \brief (EFC0      ) Base Address */
#define EFC1       ((Efc    *)0x400E0C00U) /**< \brief (EFC1      ) Base Address */

#define PIOA       ((Pio    *)0x400E0E00U) /**< \brief (PIOA      ) Base Address */
#define PIOB       ((Pio    *)0x400E1000U) /**< \brief (PIOB      ) Base Address */
#define PIOC       ((Pio    *)0x400E1200U) /**< \brief (PIOC      ) Base Address */
#define PIOD       ((Pio    *)0x400E1400U) /**< \brief (PIOD      ) Base Address */

#define RSTC       ((Rstc   *)0x400E1A00U) /**< \brief (RSTC      ) Base Address */
#define SUPC       ((Supc   *)0x400E1A10U) /**< \brief (SUPC      ) Base Address */
#define RTT        ((Rtt    *)0x400E1A30U) /**< \brief (RTT       ) Base Address */
#define WDT        ((Wdt    *)0x400E1A50U) /**< \brief (WDT       ) Base Address */
#define RTC        ((Rtc    *)0x400E1A60U) /**< \brief (RTC       ) Base Address */
#define GPBR       ((Gpbr   *)0x400E1A90U) /**< \brief (GPBR      ) Base Address */

/* ************************************************************************** */
/*   PIO DEFINITIONS FOR SAM3X8E */
/* ************************************************************************** */
/** \addtogroup SAM3X8E_pio Peripheral Pio Definitions */
/*@{*/

#include "pio_sam3x8e.h"
/*@}*/

/* ************************************************************************** */
/*   MEMORY MAPPING DEFINITIONS FOR SAM3X8E */
/* ************************************************************************** */

#define IFLASH0_SIZE             (0x40000u)
#define IFLASH0_PAGE_SIZE        (256u)
#define IFLASH0_LOCK_REGION_SIZE (16384u)
#define IFLASH0_NB_OF_PAGES      (1024u)
#define IFLASH1_SIZE             (0x40000u)
#define IFLASH1_PAGE_SIZE        (256u)
#define IFLASH1_LOCK_REGION_SIZE (16384u)
#define IFLASH1_NB_OF_PAGES      (1024u)
#define IRAM0_SIZE               (0x10000u)
#define IRAM1_SIZE               (0x8000u)
#define NFCRAM_SIZE              (0x1000u)
#define IFLASH_SIZE              (IFLASH0_SIZE+IFLASH1_SIZE)
#define IRAM_SIZE                (IRAM0_SIZE+IRAM1_SIZE)

#define IFLASH0_ADDR    (0x00080000u) /**< Internal Flash 0 base address */
#if defined IFLASH0_SIZE
#define IFLASH1_ADDR    (IFLASH0_ADDR+IFLASH0_SIZE) /**< Internal Flash 1 base address */
#endif
#define IROM_ADDR       (0x00100000u) /**< Internal ROM base address */
#define IRAM0_ADDR      (0x20000000u) /**< Internal RAM 0 base address */
#define IRAM1_ADDR      (0x20080000u) /**< Internal RAM 1 base address */
#define NFC_RAM_ADDR    (0x20100000u) /**< NAND Flash Controller RAM base address */
#define UOTGHS_RAM_ADDR (0x20180000u) /**< USB On-The-Go Interface RAM base address */
#define EBI_CS0_ADDR    (0x60000000u) /**< EBI Chip Select 0 base address */
#define EBI_CS1_ADDR    (0x61000000u) /**< EBI Chip Select 1 base address */
#define EBI_CS2_ADDR    (0x62000000u) /**< EBI Chip Select 2 base address */
#define EBI_CS3_ADDR    (0x63000000u) /**< EBI Chip Select 3 base address */
#define EBI_CS4_ADDR    (0x64000000u) /**< EBI Chip Select 4 base address */
#define EBI_CS5_ADDR    (0x65000000u) /**< EBI Chip Select 5 base address */
#define EBI_CS6_ADDR    (0x66000000u) /**< EBI Chip Select 6 base address */
#define EBI_CS7_ADDR    (0x67000000u) /**< EBI Chip Select 7 base address */

/* ************************************************************************** */
/*   ELECTRICAL DEFINITIONS FOR SAM3X8E */
/* ************************************************************************** */

/* Device characteristics */
#define CHIP_FREQ_SLCK_RC_MIN           (20000UL)
#define CHIP_FREQ_SLCK_RC               (32000UL)
#define CHIP_FREQ_SLCK_RC_MAX           (44000UL)
#define CHIP_FREQ_MAINCK_RC_4MHZ        (4000000UL)
#define CHIP_FREQ_MAINCK_RC_8MHZ        (8000000UL)
#define CHIP_FREQ_MAINCK_RC_12MHZ       (12000000UL)
#define CHIP_FREQ_CPU_MAX               (84000000UL)
#define CHIP_FREQ_XTAL_32K              (32768UL)
#define CHIP_FREQ_XTAL_12M              (12000000UL)

/* Embedded Flash Write Wait State */
#define CHIP_FLASH_WRITE_WAIT_STATE     (6U)

/* Embedded Flash Read Wait State (VDDCORE set at 1.65V) */
#define CHIP_FREQ_FWS_0                 (22500000UL) /**< \brief Maximum operating frequency when FWS is 0 */
#define CHIP_FREQ_FWS_1                 (34000000UL) /**< \brief Maximum operating frequency when FWS is 1 */
#define CHIP_FREQ_FWS_2                 (53000000UL) /**< \brief Maximum operating frequency when FWS is 2 */
#define CHIP_FREQ_FWS_3                 (78000000UL) /**< \brief Maximum operating frequency when FWS is 3 */


#endif /* _SAM3X8E_ */
