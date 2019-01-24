#include "variant.h"
//#include "pio_sam3x8e.h"

uint8_t g_pinStatus[PINS_COUNT] = {0};

void __libc_init_array(void);

#define ADC_MR_SETTLING_AST17 (0x3u << 20) /**< \brief (ADC_MR) 17 periods of ADCClock */
#define ADC_SETTLING_TIME_3 ADC_MR_SETTLING_AST17
#define   ADC_MR_TRGEN_DIS (0x0u << 0) /**< \brief (ADC_MR) Hardware triggers are disabled. Starting a conversion is only possible by software. */
#define ADC_TRIG_SW ADC_MR_TRGEN_DIS

void variantInit( void )
{
  SystemInit();

  // Set Systick to 1ms interval, common to all SAM3 variants
  if (SysTick_Config(SystemCoreClock / 1000))
  {
    // Capture error
    while (true);
  }

  // Initialize C library
  __libc_init_array();

  // Disable pull-up on every pin
  for (unsigned i = 0; i < PINS_COUNT; i++)
	  digitalWrite(i, LOW);

  // Enable parallel access on PIO output data registers
  PIOA->PIO_OWER = 0xFFFFFFFF;
  PIOB->PIO_OWER = 0xFFFFFFFF;
  PIOC->PIO_OWER = 0xFFFFFFFF;
  PIOD->PIO_OWER = 0xFFFFFFFF;

  // Initialize CAN pins
  PIO_Configure(
    g_APinDescription[PINS_CAN0].pPort,
    g_APinDescription[PINS_CAN0].ulPinType,
    g_APinDescription[PINS_CAN0].ulPin,
    g_APinDescription[PINS_CAN0].ulPinConfiguration);
  PIO_Configure(
    g_APinDescription[PINS_CAN1].pPort,
    g_APinDescription[PINS_CAN1].ulPinType,
    g_APinDescription[PINS_CAN1].ulPin,
    g_APinDescription[PINS_CAN1].ulPinConfiguration);

  // Initialize Analog Controller
  pmc_enable_periph_clk(ID_ADC);
  adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST);
  adc_configure_timing(ADC, 0, ADC_SETTLING_TIME_3, 1);
  adc_configure_trigger(ADC, ADC_TRIG_SW, 0); // Disable hardware trigger.
  adc_disable_interrupt(ADC, 0xFFFFFFFF); // Disable all ADC interrupts.
  adc_disable_all_channel(ADC);

  // Initialize analogOutput module
  analogOutputInit();
}
