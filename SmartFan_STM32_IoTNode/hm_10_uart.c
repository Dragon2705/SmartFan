
#include "hm_10_uart.h"
#include "stm32f4xx.h"


#define AF07 0x07


static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk,  uint32_t BaudRate);


void hm10_uart_init(uint32_t baud,uint32_t freq)
{
	/*Enable clock access to GPIOA and USART1*/
	RCC->APB2ENR|=RCC_APB2ENR_USART1EN;
	RCC->AHB1ENR|=RCC_AHB1ENR_GPIOAEN;
	/*Configure the GPIO for UART Mode*/
	GPIOA->MODER|=GPIO_MODER_MODE9_1;
	GPIOA->MODER&=~GPIO_MODER_MODE9_0;
	GPIOA->MODER|=GPIO_MODER_MODE10_1;
	GPIOA->MODER&=~GPIO_MODER_MODE10_0;
	GPIOA->AFR[1]|=(AF07<<GPIO_AFRH_AFSEL9_Pos)|(AF07<<GPIO_AFRH_AFSEL10_Pos); //ALT7 for UART1 (PA9 and PA10)
	/*Configure UART*/
	uart_set_baudrate(USART1,freq,baud);
	USART1->CR1|=USART_CR1_TE | USART_CR1_RE;
	USART1->CR1|=USART_CR1_UE;
	USART1->CR1|=USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);
	
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t BaudRate)
{
	return ((PeriphClk + (BaudRate/2U))/BaudRate);
}

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk,  uint32_t BaudRate)
{
	USARTx->BRR =  compute_uart_bd(PeriphClk,BaudRate);
}


void hm10_write_char(char ch)
{
	/*Make sure the transmit data register is empty*/
	while(!(USART1->SR & USART_SR_TXE)){}

	/*Write to transmit data register*/
	USART1->DR  =  (ch & 0xFF);
}


void hm10_write_at_command(char * ch)
{
	while(*ch)
	{
		hm10_write_char(*ch);
		ch++;
	}
}

void hm10_write_string(char * ch)
{
	while(*ch)
	{
		hm10_write_char(*ch);
		ch++;
	}
}
