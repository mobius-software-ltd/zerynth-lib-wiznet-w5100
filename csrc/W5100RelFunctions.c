#include "HAL_Config.h"
#include "W5100RelFunctions.h"
#include "stm_lib/inc/stm32f10x_gpio.h"
#include "stm_lib/inc/stm32f10x_spi.h"

void W5100Initialze(void)
{
	intr_kind temp;
	unsigned char W5100_AdrSet[2][4] = {{2,2,2,2},{2,2,2,2}};
	temp = IK_DEST_UNREACH;

	if(ctlwizchip(CW_INIT_WIZCHIP,(void*)W5100_AdrSet) == -1)
	{
		printf("W5100 initialized fail.\r\n");
	}

	if(ctlwizchip(CW_SET_INTRMASK,&temp) == -1)
	{
		printf("W5100 interrupt\r\n");
	}
}

uint8_t spiReadByte(void)
{
	while (SPI_I2S_GetFlagStatus(W5100_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(W5100_SPI, 0xff);
	while (SPI_I2S_GetFlagStatus(W5100_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(W5100_SPI);
}

void spiWriteByte(uint8_t byte)
{
	while (SPI_I2S_GetFlagStatus(W5100_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(W5100_SPI, byte);
	while (SPI_I2S_GetFlagStatus(W5100_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	SPI_I2S_ReceiveData(W5100_SPI);
}

inline void csEnable(void)
{
	GPIO_ResetBits(W5100_CS_PORT, W5100_CS_PIN);
}

inline void csDisable(void)
{
	GPIO_SetBits(W5100_CS_PORT, W5100_CS_PIN);
}

inline void resetAssert(void)
{
	GPIO_ResetBits(W5100_RESET_PORT, W5100_RESET_PIN);
}

inline void resetDeassert(void)
{
	GPIO_SetBits(W5100_RESET_PORT, W5100_RESET_PIN);
}

void W5100Reset(void)
{
	int i,j,k;
	k=0;
	GPIO_ResetBits(W5100_RESET_PORT,W5100_RESET_PIN);
	CoTickDelay(10);
	GPIO_SetBits(W5100_RESET_PORT,W5100_RESET_PIN);
}
