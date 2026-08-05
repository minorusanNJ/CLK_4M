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
	clk_4m_init();
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
		count_diff = last_count + 4000 - current_count;
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
