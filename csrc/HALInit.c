#include "HAL_Config.h"

#include "HALInit.h"
#include "misc.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include <stdio.h>

volatile unsigned long globalTimer = 0;

void HardFault_Handler(void)
{
	printf("Hard Fault!!!\r\n");
	while(1);
}

void MemManage_Handler(void)
{
	printf("Mem Manage!!!\r\n");
	while(1);
}

void NMI_Handler(void)
{
	printf("NMI Handler!!!\r\n");
	while(1);
}

void BusFault_Handler(void)
{
	printf("Bus Fault!!!\r\n");
	while(1);
}

void UsageFault_Handler(void)
{
	printf("Bus Fault!!!\r\n");
	while(1);
}

void timerInitialize(void)
{
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

        TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
        TIM_TimeBaseInitStruct.TIM_Period = 8400-1;
        TIM_TimeBaseInitStruct.TIM_Prescaler = 1000-1;
        TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
        TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
        TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
        TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
        TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
        TIM_SetAutoreload(TIM2,8400-1);
        TIM_Cmd(TIM2,ENABLE);

        NVIC_InitTypeDef   NVIC_InitStructure;

        NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
}

void TIM2_IRQHandler(void){
        TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
        globalTimer++;
}


void TIM2_settimer(void)
{
	TIM_Cmd(TIM2,DISABLE);
	globalTimer = 0;
	TIM_Cmd(TIM2,ENABLE);
}

unsigned long TIM2_gettimer(void)
{
	return globalTimer;
}

void clockConfiguration(void)
{

}

void gpioInitialize(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/*GPIO For USART initialize*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/* Configure USART Tx */
	GPIO_InitStructure.GPIO_Pin = PRINTF_USART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(PRINTF_USART_TX_PORT, &GPIO_InitStructure);

	/* Configure USART Rx */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = PRINTF_USART_RX_PIN;

	GPIO_Init(PRINTF_USART_RX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIO for W5100 initialize */
	RCC_APB2PeriphClockCmd(W5100_GPIO_RCC, ENABLE);

	/*For SPI*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = W5100_SPI_SCK_PIN | W5100_SPI_MOSI_PIN | W5100_SPI_MISO_PIN;
	GPIO_Init(W5100_SPI_PORT, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	/*For CS*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = W5100_CS_PIN;
	GPIO_Init(W5100_CS_PORT, &GPIO_InitStructure);

	/*For Reset*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = W5100_RESET_PIN;
	GPIO_Init(W5100_RESET_PORT, &GPIO_InitStructure);
}

void spiInitailize(void)
{
	/*W5100 SPI initialize*/
	RCC_APB1PeriphClockCmd(W5100_SPI_RCC, ENABLE);

	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	/* Initializes the SPI communication */
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

	SPI_Init(W5100_SPI, &SPI_InitStructure);

	SPI_Cmd(W5100_SPI,ENABLE);
}
