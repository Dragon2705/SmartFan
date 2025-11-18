#include "RccConfig.h"
#include "adc.h"



void ADC_Init (void)
{
	
	
//1. Enable ADC and GPIO clock
	RCC->APB2ENR |= (1<<8);  // enable ADC1 clock
	RCC->AHB1ENR |= (1<<0);  // enable GPIOA clock
	
//2. Set the prescalar in the Common Control Register (CCR)	
	ADC->CCR |= 1<<16;  		 // PCLK2 divide by 4
	
//3. Set the Scan Mode and Resolution in the Control Register 1 (CR1)	
	ADC1->CR1 = (1<<8);    // SCAN mode enabled
	ADC1->CR1 &= ~(1<<24);   // 8 bit RES
	ADC1->CR1 |= (1<<25);
	
//4. Set the Continuous Conversion, EOC, and Data Alignment in Control Reg 2 (CR2)
	ADC1->CR2 |= (1<<1);     // enable continuous conversion mode
	ADC1->CR2 |= (1<<10);    // EOC after each conversion
	ADC1->CR2 &= ~(1<<11);   // Data Alignment RIGHT
	
//5. Set the Sampling Time for the channels	
	ADC1->SMPR2 &= ~((1<<3) | (1<<12));  // Sampling time of 3 cycles for channel 1 and channel 4

//6. Set the Regular channel sequence length in ADC_SQR1
ADC1->SQR1 &= ~(0xF << 20); // L = 0  for 1 conversions
	
//7. Set the Respective GPIO PINs in the Analog Mode	
	GPIOA->MODER &= ~(3<<2);  // analog mode for PA 1 (delete channel 1)
	GPIOA->MODER |= (3<<8);  // analog mode for PA 4 (channel 4)
	
	// Enable DMA for ADC
ADC1->CR2 |= (1<<8);
// Enable Continuous Request
ADC1->CR2 |= (1<<9);
//ADC1->SQR3 |= (1<<0);  // SEQ1 for Channel 1
ADC1->SQR3 = 4;  // SEQ2 for CHannel 4
}


void ADC_Enable (void)
{
	/************** STEPS TO FOLLOW *****************
	1. Enable the ADC by setting ADON bit in CR2
	2. Wait for ADC to stabilize (approx 10us) 
	************************************************/
	ADC1->CR2 |= 1<<0;   // ADON =1 enable ADC1
	
	uint32_t delay = 10000;
	while (delay--);
}


void ADC_Start (void)
{
	/************** STEPS TO FOLLOW *****************
	1. Set the channel Sequence in the SQR Register
	2. Clear the Status register
	3. Start the Conversion by Setting the SWSTART bit in CR2
	************************************************/
	
	   // conversion in regular sequence
	
	ADC1->SR = 0;        // clear the status register
	
	ADC1->CR2 |= (1<<30);  // start the conversion
}


void ADC_WaitForConv (void)
{
	/*************************************************
	EOC Flag will be set, once the conversion is finished
	*************************************************/
	while (!(ADC1->SR & (1<<1)));  // wait for EOC flag to set
}

uint16_t ADC_GetVal (void)
{
	return ADC1->DR;  // Read the Data Register
}

void ADC_Disable (void)
{
	/************** STEPS TO FOLLOW *****************
	1. Disable the ADC by Clearing ADON bit in CR2
	************************************************/	
	ADC1->CR2 &= ~(1<<0);  // Disable ADC
}

void DMA_Init (void)
{
	//Enable DMA2 clock
	RCC->AHB1ENR |= (1<<22);
	//select the data direction
	DMA2_Stream0->CR &= ~(3<<6);  //00: Peripheral-to-memory
	DMA2_Stream0->CR |= (1<<8); // Circular mode enabled
	DMA2_Stream0->CR |= (1<<10); // Memory address pointer is incremented after each data transfer 
	
	//Set size for data
	DMA2_Stream0->CR &= ~((1<<11)|(1<<13)); //size 8 bit data
	DMA2_Stream0->CR &= ~(7<<25); //select chanel 0
	
}

void DMA_Config (uint32_t srcAdd, uint32_t destAdd, uint32_t size)
{
	DMA2_Stream0->NDTR = size;
	DMA2_Stream0->PAR = srcAdd;
	DMA2_Stream0->M0AR = destAdd;
	DMA2_Stream0->CR |= (1<<0);

}	