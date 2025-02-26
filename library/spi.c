/*
 * spi.c
 *
 *  Created on: Feb 21, 2025
 *      Author: wataoxp
 */
#include "spi.h"
#include "delay.h"

ErrorStatus SPI_Init(SPI_TypeDef *SPIx,SPI_InitTypedef *config)
{
	uint16_t CR1Reg = config->TransferDirection |  config->Mode |config->ClockPolarity | \
    		config->ClockPhase | config->NSS |  config->BaudRate | config->BitOrder;

	if(LL_SPI_IsEnabled(SPIx) != 0)
	{
		return ERROR;
	}
	WRITE_REG(SPIx->CR1,CR1Reg);

    MODIFY_REG(SPIx->CR2,SPI_CR2_DS | SPI_CR2_SSOE,config->DataWidth | config->SSOE);

    if(config->DataWidth < LL_SPI_DATAWIDTH_9BIT)		//データサイズが8ビット以下ならFRXTHをセットする
    {
    	LL_SPI_SetRxFIFOThreshold(SPIx, LL_SPI_RX_FIFO_TH_QUARTER);
    }
    LL_SPI_DisableCRC(SPIx);

    LL_I2S_Disable(SPIx);								//I2SとSPIは排他的な関係

	LL_SPI_SetStandard(SPIx, LL_SPI_PROTOCOL_MOTOROLA);
	LL_SPI_DisableNSSPulseMgt(SPIx);

    return SUCCESS;
}
void SPI_StructInit(SPI_InitTypedef *config,uint32_t SPI_Mode)
{
	config->TransferDirection = LL_SPI_FULL_DUPLEX;
	config->DataWidth = LL_SPI_DATAWIDTH_8BIT;
	config->ClockPolarity = LL_SPI_POLARITY_LOW;
	config->ClockPhase = LL_SPI_PHASE_1EDGE;
	config->BitOrder = LL_SPI_MSB_FIRST;

	if(SPI_Mode == MSTR_MASTER)
	{
		config->Mode = MSTR_MASTER;
		config->SSOE = SSOE_OUTPUT_ENSBLE;
		config->BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;		//SPIクロック周波数
	}
	else
	{
		config->Mode = MSTR_SLAVE;
		config->NSS = NSS_AUTO_CONTROL;
	}
}
void SPI_Transmit8(SPI_TypeDef *SPIx,uint8_t *buf,uint16_t length)
{
	LL_SPI_Enable(SPIx);

	while(LL_SPI_IsActiveFlag_BSY(SPIx));

	while(length--)
	{
		LL_SPI_TransmitData8(SPIx, *buf++);
		while(LL_SPI_IsActiveFlag_TXE(SPIx) == 0);
		Delay(1);
	}
	LL_SPI_Disable(SPIx);
}
void SPI_MasterTransmitReceive8(SPI_TypeDef *SPIx,uint8_t *TXbuf,uint8_t *Rxbuf,uint32_t size)
{
	LL_SPI_Enable(SPIx);

	while(LL_SPI_IsActiveFlag_BSY(SPIx));

	for(uint32_t i = 0;i < size; i++)
	{
		LL_SPI_TransmitData8(SPIx, TXbuf[i]);
		while(LL_SPI_IsActiveFlag_TXE(SPIx) == 0);
		while(LL_SPI_IsActiveFlag_RXNE(SPIx) == 0);
		Rxbuf[i] = LL_SPI_ReceiveData8(SPIx);

		//Delay(1);
	}

	LL_SPI_Disable(SPIx);
}
