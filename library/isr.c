/*
 * isr.c
 *
 *  Created on: Feb 4, 2025
 *      Author: wataoxp
 */
//#define I2C_HANDLE
#define SPI_HANDLE

#include "isr.h"
#ifdef I2C_HANDLE
#include "i2c.h"
#endif
#ifdef SPI_HANDLE
#include "spi.h"
#endif

/* SysTick ISR */
static uint32_t Tick;

void OverRunHandler(void)
{
	while(1);
}

void SysTick_Handler(void)
{
	Tick++;
}
void SetTick(uint32_t ntick)
{
	Tick = ntick;
}
uint32_t GetTick(void)
{
	return Tick;
}
#ifdef I2C_HANDLE
/* I2C ISR */
void I2C1_IRQHandler(void)
{
	if(I2C_Slave_IT(I2C1) != 0)
	{
		OverRunHandler();
	}
}
void I2C2_IRQHandler(void)
{
	if(I2C_Master_IT(I2C2) != 0)
	{
		OverRunHandler();
	}
}
#endif

#ifdef SPI_HANDLE
void SPI1_IRQHandler(void)
{
	;
}
uint8_t sdbuf[20] = {10,11,12,13,14,15,16,17,18,19};
uint8_t rcbuf[20];
uint8_t size;
void SPI2_IRQHandler(void)
{
	if(LL_SPI_IsActiveFlag_RXNE(SPI2))
	{
		rcbuf[size] = LL_SPI_ReceiveData8(SPI2);
		LL_SPI_TransmitData8(SPI2, sdbuf[size]);
		if(size < 10)
		{
			size++;
		}
		else
		{
			size = 0;
		}
	}
}
#endif
