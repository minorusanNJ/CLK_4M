#include "led.h"

#define		GPIOAEN				(1U<<0)
#define		MODER_PA5_RESET				~(3U<<(5*2))
#define		MODER_PA5_OUTPUT	(1U<<(5*2))

void led_init(void)
{
	RCC->AHB1ENR	|=	GPIOAEN;
	GPIOA->MODER	&=			MODER_PA5_RESET;
	GPIOA->MODER	|=	MODER_PA5_OUTPUT;
	//GPIOA->ODR		|=	ODR_PA5_H;
}
