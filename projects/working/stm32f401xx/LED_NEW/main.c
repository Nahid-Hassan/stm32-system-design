#include "stm32f401xc.h"

void SysClockConfig(void);
void GPIOConfig(void);
void Delay(uint32_t time);

void SysClockConfig(void) {
	
	#define PLL_M 4
	#define PLL_N 84
	#define PLL_P 0   /* 2 -> 0, pass 0 in 16 position */
	

	/* Enable HSE & wait for HSE to Ready*/
	RCC->CR |= RCC_CR_HSEON;  /* 1U << 16U */
	/* while(!(RCC->CR & (1U << 17U))); // 17th bit 1 means HSE is ready */
	while(!(RCC->CR & RCC_CR_HSERDY));
	
	/* Enable power enable clock and voltage regulator */
	/* RCC_APB1ENR -> Peripheral clock enable register */
	RCC->APB1ENR |= RCC_APB1ENR_PWREN; /* 1U << 28U; set 28th Power ON bit */
	
	/* voltage regulator */
	/* PWR->CR |= 3U << 14U;           // ref: page 87 */ 
	PWR->CR |= PWR_CR_VOS;
	
	/* configure flash prefetch & latency related settings */
	/* 8 -> PRFTEN, 9 -> ICEN, 10 -> DCEN */
	/* FLASH->ACR = (1U << 8U) | (1U << 9U) | (1U << 10) | ; */
	FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_2WS;
	
	/* Configure Prescaler (Main Clock & Peripheral Clock */
	
	/* 
	** AP1 Prescaler is 2
	** AP2 Prescaler is 1
	** AHB Prescaler is 1
	*/
	/* RCC Clock Configuration Register "RCC_CFGR" */
	/* AHB PR */
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	
	/* APB1 PR */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
	
	/* APB2 PR */
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

	/* Configure the Main PLL */
	/* 
	** PLLM -> 4      , start bit 0
	** PLLN -> 84     , start bit 6
	** PLLP -> 2      , start bit 16
	*/
	RCC->PLLCFGR = (PLL_M << 0U) | (PLL_N << 6U) | (PLL_P << 16U) | (RCC_PLLCFGR_PLLSRC_HSE);
	
	/* Enable the PLL */
	/*
	 ** In RCC->CR Register, Bit 24 is for PLLON and
	 ** bit 25 is for PLLRDY
	*/
	RCC->CR |= RCC_CR_PLLON;
	/* wait for PLLRDY */
	while(!(RCC->CR & (RCC_CR_PLLRDY)));

	/* Set the clock source */
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void GPIOConfig(void) {
  /*
	** Enable GPIO Clock
	** Set the pin as Output
	** Configure the Output Mode
	*/
	
	RCC->AHB1ENR |= (1U << 0U); /* IO port A clock enable */
	
	/* set the pin as output */
	GPIOA->MODER |= (1U << 10U); /* set 1 in 10th position, 11th position is zero by default */

	/* configure output mode */
	GPIOA->OTYPER = 0;
	GPIOA->OSPEEDR = 0;
}

void Delay(uint32_t time) {
	while(time) {
		time--;
	}
}

int main(void) {
	SysClockConfig();
	GPIOConfig();
	
	while (1) {
		GPIOA->BSRR |= (1 << 5); /* SET THE PIN PA5 */
		Delay(1000000);
		GPIOA->BSRR |= ((1<<5) << 16); /* RESET PIN PA5 */
		Delay(1000000);
	}
}
