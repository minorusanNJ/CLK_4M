#include "tim2.h"	//apb1 pa0 tim2 af01
//RCC
#define	TIM2EN		(1U<<0)
#define GPIOAEN		(1U<<0)				//Not in use
//TIM2
#define	CR1_CEN		(1U<<0)
#define DIER_UIE	(1U<<0)
//to .h #define SR_UIF		(1U<<0)
#define PSCV		0
#define ARRV		1332


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
