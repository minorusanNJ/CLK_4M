最初にLEDの点滅を行う。

クロック周波数を4MHzに設定
	Flashのlatencyを0に設定
	水晶発振子を使用
		設定後は発振が安定するまで待つ
	PLLがOFFの時はパワーコントロールレジスタのVOSは64MHz以下に設定されるので、
	パワーインターフェースを使用しない
	各バスの最高速度以内なので、何もしない
	PLLを使用しない、クロックツリーに備わっているHSE分周器を用いる。
	これは、ＣＦＧＲのバス分周の事。AHB1が4MHzになると、APB1、APB2共自動で4MHzになる。
//clk.h /////////////////////////////////////////
#ifndef __CLK_H__
#define __CLK_H__

#include "stm32f4xx.h"

void clk_4m_init(void);		//⇚変更

#endif
//clk.c ///////////////////////////////////////// 大幅削除・変更
#include "clk.h"
//FLASH access control register in reference manual
#define	FLASH_LATENCY_RESET			~(0xF<<0)
//RCC reset and clock control in reference manual
#define CR_HSEON			(1U<<16)
#define CR_HSERDY			(1U<<17)
//RCC reset and clock control in reference manual
#define CFGR_PPRE2					~(1U<<15)	//NOT DEVIDED
#define CFGR_PPRE1					~(1U<<12)	//NOT DEVIDED
#define CFGR_HPRE			(8U<<4)				// SYSTEM CLOCK DEVIDED BY 2
#define	CFGR_SW_RESET				~(3U<<0)
#define	CFGR_SWS_RESET				~(3U<<2)
#define	CFGR_SW				(1U<<0)
#define	CFGR_SWS			(1U<<2)
#define CR_HSIOFF					~(1U<<0)

void clk_4m_init(void)
{
	FLASH->ACR		&=			FLASH_LATENCY_RESET;
	RCC->CR			|=	CR_HSEON;
	while(!(RCC->CR & CR_HSERDY)){}
	RCC->CFGR		&=			(CFGR_PPRE2 & CFGR_PPRE1);
	RCC->CFGR		|=	(CFGR_HPRE | CFGR_SW);
	while(!(RCC->CFGR & CFGR_SWS)){}
	RCC->CR			&=			CR_HSIOFF;
}
//systick.c /////////////////////////////////////
#include "systick.h"

//SysTick	CortexM4GenericUserGuide
//#define	COUNTFLAG		(1U<<16)
#define CLKSOURCE		(1U<<2)
#define TICKINT			(1U<<1)
#define	ENABLE			(1U<<0)
#define SYSCLK			4000000		//⇚変更

void systick_1ms_timer(void)
{
	SysTick->LOAD	=	(SYSCLK * 0.001) - 1;
	SysTick->VAL	=	0;
	SysTick->CTRL	|=	(CLKSOURCE | TICKINT | ENABLE);
	NVIC_EnableIRQ(SysTick_IRQn);
}
//tim2.c ////////////////////////////////////////
#include "tim2.h"	//apb1 pa0 tim2 af01
//RCC
#define	TIM2EN		(1U<<0)
#define GPIOAEN		(1U<<0)				//Not in use
//TIM2
#define	CR1_CEN		(1U<<0)
#define DIER_UIE	(1U<<0)
//to .h #define SR_UIF		(1U<<0)
#define PSCV		0
#define ARRV		1332				//⇚変更


void tim2_3khz_init(void)
{
	RCC->AHB1ENR	|=	GPIOAEN;		//Not in use
	RCC->APB1ENR	|=	TIM2EN;
	TIM2->ARR		= ARRV;
	TIM2->PSC		= PSCV;
	TIM2->DIER		|=	DIER_UIE;
	TIM2->CR1		|=	CR1_CEN;
	NVIC_EnableIRQ(TIM2_IRQn);
}
//uart.c ////////////////////////////////////////
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
#define APB1_CLK		4000000UL	//⇚変更
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
//main.c ////////////////////////////////////////
#include "led.h"
#include "clk.h"
#include "systick.h"
#include "uart.h"
#include <stdio.h>
#include "tim2.h"

volatile uint16_t	mill_sec = 0;
volatile uint8_t	timer1 = 0;
volatile uint8_t	printout = 0;
volatile uint32_t	last_count = 0;
volatile uint32_t	count_diff = 0;

int main(void)
{
	led_init();
	clk_4m_init();				//⇚変更
	systick_1ms_timer();
	uart_init();
	tim2_3khz_init();

	while(1)
	{/*
		if(timer1)
		{
			printf("UART is working... \n\r");
			timer1 = 0;
		}
	 */
		if(printout)
		{
			printf("Count_diff %ld ......\n\r", count_diff);
			printout = 0;
		}
	}
}
void SysTick_Handler(void)
{
	mill_sec ++;
	if(mill_sec >= 1000)
	{
		mill_sec = 0;
		GPIOA->ODR	^=	ODR_PA5_H;
		//timer1 = 1;
	}
}
void call_back_tim2(void)
{
	uint32_t current_count = SysTick->VAL;	//down conter
	if(last_count >= current_count)
	{
		count_diff = last_count - current_count;
	}
	else
	{
		count_diff = last_count + 4000 - current_count;　//⇚変更
	}
	last_count = current_count;
	printout = 1;
}
void TIM2_IRQHandler(void)
{
	if(TIM2->SR & SR_UIF)
	{
		TIM2->SR &= 		~SR_UIF;
		call_back_tim2();
	}
}

















