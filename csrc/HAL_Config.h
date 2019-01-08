#ifndef __HAL_CONFIG_H_
#define __HAL_CONFIG_H_

#include "stm32f10x_rcc.h"

#define W5100_GPIO_RCC			RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF
#define W5100_RESET_PIN		  GPIO_Pin_8
#define W5100_RESET_PORT		GPIOD
#define W5100_CS_PIN			  GPIO_Pin_7
#define W5100_CS_PORT			  GPIOD
#define W5100_INT_PIN			  GPIO_Pin_9
#define W5100_INT_PORT			GPIOD

#define W5100_SPI				    SPI2
#define W5100_SPI_RCC			  RCC_APB1Periph_SPI2
#define W5100_SPI_PORT	    GPIOB
