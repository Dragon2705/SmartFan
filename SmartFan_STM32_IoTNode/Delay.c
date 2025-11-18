


#include "Delay.h"
#include "RccConfig.h"

void TIM5Config (void)
{
	
	RCC->APB1ENR |= (1<<3);  // Enable the timer5 clock
	
// 2. Set the prescalar and the ARR
	TIM5->PSC = 84-1;  // 84MHz/84 = 1 MHz ~~ 1 uS delay
	TIM5->ARR = 0xffff;  // MAX ARR value
	
// 3. Enable the Timer, and wait for the update Flag to set
	TIM5->CR1 |= (1<<0); // Enable the Counter
	while (!(TIM5->SR & (1<<0)));  // UIF: Update interrupt flag..  This bit is set by hardware when the registers are updated
}

void Delay_us (uint16_t us)
{
	
	TIM5->CNT = 0;
	while (TIM5->CNT < us);
}

void Delay_ms (uint16_t ms)
{
	for (uint16_t i=0; i<ms; i++)
	{
		Delay_us (1000); // delay of 1 ms
	}
}

