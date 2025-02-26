#include "main.h"

//#define SPI_SINGLE_MODE

static void RCC_Config(void);
static void SPI1_Config(void);
static void SPI2_Config(void);

int main(void)
{
	uint8_t send[] = {0,1,2,3,4,5,6,7,8,9};
	uint8_t recv[20];

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	RCC_Config();
	SysTick_Init(MILL_SECOND);
	GPIO_InputInit(GPIOA, PORTA, Pin3, LL_GPIO_MODE_INPUT, LL_GPIO_PULL_UP);

	SPI1_Config();
#ifdef SPI_SINGLE_MODE
	SPI2_Config();

	__NVIC_SetPriority(SPI2_IRQn, HighPriority);
	__NVIC_EnableIRQ(SPI2_IRQn);

	LL_SPI_EnableIT_RXNE(SPI2);
	LL_SPI_Enable(SPI2);
#endif

	while(1)
	{
		while(READ_BIT(GPIOA->IDR,1 << Pin3));
		SPI_MasterTransmitReceive8(SPI1, send, recv, 10);
		Delay(1000);
	}
}
static void RCC_Config(void)
{
	RCC_InitTypedef rcc;
	rcc.Latency = LL_FLASH_LATENCY_2;
	rcc.PLLSrc = LL_RCC_PLLSOURCE_HSI;
	rcc.PLLM = LL_RCC_PLLM_DIV_1;
	rcc.PLLN = 8;
	rcc.PLLR = LL_RCC_PLLR_DIV_2;
	rcc.AHBdiv = LL_RCC_SYSCLK_DIV_1;
	rcc.SysClkSrc = LL_RCC_SYS_CLKSOURCE_PLL;
	rcc.APBdiv = LL_RCC_APB1_DIV_1;
	rcc.clock = 64000000;

	RCC_InitG0(&rcc);
}
static void SPI1_Config(void)
{
	GPIO_InitTypedef init = {
			.Mode = LL_GPIO_MODE_ALTERNATE,
			.Speed = LL_GPIO_SPEED_FREQ_LOW,
			.OutputType = LL_GPIO_OUTPUT_PUSHPULL,
			.Pull = LL_GPIO_PULL_NO,
			.Alternate = LL_GPIO_AF_0,
	};
	SPI_InitTypedef config = {0};

	if(LL_IOP_GRP1_IsEnabledClock(1 << PORTA) == 0)
	{
		LL_IOP_GRP1_EnableClock(1 << PORTA);
	}

	init.PinPos = Pin1;			//SCK
	GPIO_OutputInit(GPIOA, &init);
	init.PinPos = Pin2;			//MOSI
	GPIO_OutputInit(GPIOA, &init);
	init.PinPos = Pin4;			//NSS
	GPIO_OutputInit(GPIOA, &init);
	init.PinPos = Pin6;			//MISO
	GPIO_OutputInit(GPIOA, &init);

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

//	config.TransferDirection = LL_SPI_FULL_DUPLEX;
//	config.Mode = SPI_CR1_MSTR;
//	config.ClockPolarity = LL_SPI_POLARITY_LOW;
//	config.ClockPhase = LL_SPI_PHASE_1EDGE;
//	config.NSS = NSS_MASTER_HARD_OUTPUT;
//	config.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;
//	config.BitOrder = LL_SPI_MSB_FIRST;
//
//	config.SSOE = SPI_CR2_SSOE;
//	config.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	SPI_StructInit(&config, MSTR_MASTER);

	SPI_Init(SPI1, &config);
}
#ifdef SPI_SINGLE_MODE
static void SPI2_Config(void)
{
	GPIO_InitTypedef init = {
			.Mode = LL_GPIO_MODE_ALTERNATE,
			.Speed = LL_GPIO_SPEED_FREQ_LOW,
			.OutputType = LL_GPIO_OUTPUT_PUSHPULL,
			.Pull = LL_GPIO_PULL_NO,
			.Alternate = LL_GPIO_AF_0,
	};
	SPI_InitTypedef config = {0};

	LL_IOP_GRP1_EnableClock(1 << PORTA);
	LL_IOP_GRP1_EnableClock(1 << PORTB);

	init.PinPos = Pin0;			//SCK
	GPIO_OutputInit(GPIOA, &init);
	init.PinPos = Pin7;			//MOSI
	init.Alternate = LL_GPIO_AF_1;
	GPIO_OutputInit(GPIOB, &init);
	init.PinPos = Pin9;			//NSS
	init.Alternate = LL_GPIO_AF_5;
	GPIO_OutputInit(GPIOB, &init);
	init.PinPos = Pin2;			//MISO
	init.Alternate = LL_GPIO_AF_1;
	GPIO_OutputInit(GPIOB, &init);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

	SPI_StructInit(&config, MSTR_SLAVE);

	SPI_Init(SPI2, &config);
}
#endif
