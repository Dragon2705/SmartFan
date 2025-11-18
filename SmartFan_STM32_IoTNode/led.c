/*
 * led.c
 *
 *  Created on: Jun 7, 2023
 *      Author: hussamaldean
 */


#include "led.h"
#include "stm32f4xx.h"


void led_init(void)
{
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER|=GPIO_MODER_MODE5_0;
	GPIOA->MODER&=~GPIO_MODER_MODE5_1;

}

void led_on(void)
{
	GPIOA->BSRR=GPIO_BSRR_BS5;
}

void led_off(void)
{
	GPIOA->BSRR=GPIO_BSRR_BR5;
}

void led_toggle(void)
{
	GPIOA->ODR^=GPIO_ODR_OD5;
}
