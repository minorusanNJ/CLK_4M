#include "systick.h"

//SysTick	CortexM4GenericUserGuide
//#define	COUNTFLAG		(1U<<16)
#define CLKSOURCE		(1U<<2)
#define TICKINT			(1U<<1)
#define	ENABLE			(1U<<0)
#define SYSCLK			4000000

void systick_1ms_timer(void)
{
	SysTick->LOAD	=	(SYSCLK * 0.001) - 1;
	SysTick->VAL	=	0;
	SysTick->CTRL	|=	(CLKSOURCE | TICKINT | ENABLE);
	NVIC_EnableIRQ(SysTick_IRQn);
}
