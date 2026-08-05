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
