#include "uart.h"
//RCC
#define	USART2EN		(1U<<17)	//ALTERNATE FUNCTION 07
#define GPIOAEN			(1U<<0)
//GPIO
#define	MODER_PA2_ALT	(2U<<(2 * 2))
#define AFL_07_RESET			~(0xF<<(2 * 4))
#define AFL_07			(7U<<(2 * 4))
//USART
#define CR1_UE			(1U<<13)
#define CR1_TE			(1U<<3)
#define APB1_CLK		4000000UL
#define BAUDV			115200UL
#define	SR_TXE			(1U<<7)		//TC か　TXE　送信はTXEが効率的らしい

void uart_init(void)
{
	RCC->APB1ENR	|=	USART2EN;
	RCC->AHB1ENR	|=	GPIOAEN;
	GPIOA->MODER	|=	MODER_PA2_ALT;
	GPIOA->AFR[0]	&=			AFL_07_RESET;
	GPIOA->AFR[0]	|=	AFL_07;
	USART2->BRR		=	(uint32_t)((APB1_CLK / BAUDV) + 0.5);
	USART2->CR1		|=	(CR1_UE | CR1_TE);
}

void usart_tx_write(int ch)
{
	while(!(USART2->SR & SR_TXE)){}
	USART2->DR = (ch & 0xFF);
}
int __io_putchar(int ch)
{
	usart_tx_write(ch);
	return ch;
}







