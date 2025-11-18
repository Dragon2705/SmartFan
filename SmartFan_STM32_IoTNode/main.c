#include "RccConfig.h"

#include "delay.h"

#include "uart.h"

#include "adc.h"

#include "timer_pwm.h"

#include "hm_10.h"

#include "hm_10_uart.h"

#include "led.h"

#include "stdio.h"



#define FAN_VALUE_0 0;
#define FAN_VALUE_1 4;
#define FAN_VALUE_2 9;


volatile uint8_t mode = 3;

volatile uint8_t RxBuffer[5];
volatile uint8_t RxBuffer1[5];
volatile uint16_t valadc;
volatile uint8_t ByteInRxBuffer = 0;
volatile uint8_t valfan;

//volatile char uart1_rec;

volatile  uint8_t ADC_VAL[2] = {0,0};
int main() {

				SysClockConfig();
        TIM5Config();
        TIM4_Config();
        Uart2Config();
        ADC_Init();
        ADC_Enable();
				DMA_Init();
				DMA_Config((uint32_t)&ADC1->DR,(uint32_t)ADC_VAL,1);
				ADC_Start();
	
//				Ticks_Init(16000000);
//				PC13_EXTI_Init();
				led_init();
				hm10_uart_init(9600,84000000);
				FactoryReset();
				moduleReset();

        while (1) {
                ADC_Start( );
                ADC_WaitForConv();
                ADC_VAL[0] = ADC_GetVal();

                //tu dong 
					
					if(mode==0){
                        if (ADC_VAL[0] >= 200) {
                                TIM4 -> CCR1 = FAN_VALUE_2;
										
                        }
                        if (ADC_VAL[0] < 200 && ADC_VAL[0] >= 100) {
                                TIM4 -> CCR1 = FAN_VALUE_1;
                        }
                        if (ADC_VAL[0] < 100) {
                                TIM4 -> CCR1 = FAN_VALUE_0;
                        }
						}
	}	
}
__WEAK void HC_10_Callback(unsigned char ch)
{
	if (ch == '1')
	{
		led_toggle();
		TIM4 -> CCR1 = FAN_VALUE_2;
	}
}
void USART1_IRQHandler (void)
{

		    RxBuffer[ByteInRxBuffer] = (uint8_t)(USART1 -> DR & (uint8_t) 0xFF);// lay Data tu Buffer
        ByteInRxBuffer++;
        if (RxBuffer[4] == 0x03) {
                ByteInRxBuffer = 0; // cho Byte ve 0
								RxBuffer[4] = 0;
                // check buffer
                if (RxBuffer[1] == 0x01)
								{
												if(RxBuffer[2]!= 0x01) {
																hm10_write_char(0x02);
																hm10_write_char(0x81);
																hm10_write_char(0x01);
																hm10_write_char(0xff);
																hm10_write_char(0x03);
													
												}else{
                        if (RxBuffer[3] == 0x01) {
                                mode = 1;
																TIM4 -> CCR1 = FAN_VALUE_2;
                                hm10_write_char(0x02);
																hm10_write_char(0x81);
																hm10_write_char(0x01);
																hm10_write_char(0x00);
																hm10_write_char(0x03);
                        } else if (RxBuffer[3] == 0x00) {
                                mode = 3; 
																TIM4 -> CCR1 = FAN_VALUE_0;
                                hm10_write_char(0x02);
																hm10_write_char(0x81);
																hm10_write_char(0x01);
																hm10_write_char(0x00);
																hm10_write_char(0x03);
                        } else {
                                hm10_write_char(0x02);
																hm10_write_char(0x81);
																hm10_write_char(0x01);
																hm10_write_char(0x01);
																hm10_write_char(0x03);
                        }

											}
                }
								else if (RxBuffer[1] == 0x02) 
								{
													if(RxBuffer[2]!= 0x01) {
																hm10_write_char(0x02);
																hm10_write_char(0x82);
																hm10_write_char(0x01);
																hm10_write_char(0xff);
																hm10_write_char(0x03);
													
												}else{
																//frame chon che do
																if (RxBuffer[3] == 0x01) {
																				if (mode == 3){
																						hm10_write_char(0x02);
																						hm10_write_char(0x82);
																						hm10_write_char(0x01);
																						hm10_write_char(0x02);
																						hm10_write_char(0x03);
																				}
																				else {
																						mode = 1;
																						hm10_write_char(0x02);
																						hm10_write_char(0x82);
																						hm10_write_char(0x01);
																						hm10_write_char(0x00);
																						hm10_write_char(0x03);
																				}
																} else if (RxBuffer[3] == 0x00) {
																				if (mode == 3){
																						hm10_write_char(0x02);
																						hm10_write_char(0x82);
																						hm10_write_char(0x01);
																						hm10_write_char(0x02);
																						hm10_write_char(0x03);
																				}
																				else {
																						mode = 0;
																						hm10_write_char(0x02);
																						hm10_write_char(0x82);
																						hm10_write_char(0x01);
																						hm10_write_char(0x00);
																						hm10_write_char(0x03);
																				}
																} else {
																						hm10_write_char(0x02);
																						hm10_write_char(0x82);
																						hm10_write_char(0x01);
																						hm10_write_char(0x01);
																						hm10_write_char(0x03);
																}
											}                      																								
              }
							else if (RxBuffer[1] == 0x03) {
								if(RxBuffer[2]!= 0x01) {
																hm10_write_char(0x02);
																hm10_write_char(0x83);
																hm10_write_char(0x01);
																hm10_write_char(0xff);
																hm10_write_char(0x03);
													
												}else{
                        //frame chon muc gio
                        if (RxBuffer[3] == 0x00) {
                                if (mode == 3){
																		hm10_write_char(0x02);
																		hm10_write_char(0x83);
																		hm10_write_char(0x01);
																		hm10_write_char(0x02);
																		hm10_write_char(0x03);
																}
                                else {
                                    TIM4 -> CCR1 = FAN_VALUE_0;
																		mode = 1;
                                    hm10_write_char(0x02);
																		hm10_write_char(0x83);
																		hm10_write_char(0x01);
																		hm10_write_char(0x00);
																		hm10_write_char(0x03);
                                }
                        } else if (RxBuffer[3] == 0x01) {
                                if (mode == 3){
																		hm10_write_char(0x02);
																		hm10_write_char(0x83);
																		hm10_write_char(0x01);
																		hm10_write_char(0x02);
																		hm10_write_char(0x03);
																}
                                else {
                                    TIM4 -> CCR1 = FAN_VALUE_1;
																		mode = 1;
                                    hm10_write_char(0x02);
																		hm10_write_char(0x83);
																		hm10_write_char(0x01);
																		hm10_write_char(0x00);
																		hm10_write_char(0x03);
                                }
                        } else if (RxBuffer[3] == 0x02) {
                                if (mode == 3){
																		hm10_write_char(0x02);
																		hm10_write_char(0x83);
																		hm10_write_char(0x01);
																		hm10_write_char(0x02);
																		hm10_write_char(0x03);
																}
                                else {
                                    TIM4 -> CCR1 = FAN_VALUE_2;
																		mode = 1;
                                    hm10_write_char(0x02);
																		hm10_write_char(0x83);
																		hm10_write_char(0x01);
																		hm10_write_char(0x00);
																		hm10_write_char(0x03);
                                }
                        } else {
                                hm10_write_char(0x02);
																hm10_write_char(0x83);
																hm10_write_char(0x01);
																hm10_write_char(0x01);
																hm10_write_char(0x03);
                        }
                     
											}
												
                }
        }
				if (RxBuffer[1] == 0x04 && RxBuffer[3] == 0x03) {
											ByteInRxBuffer = 0;
											RxBuffer[3] = 0;
											if(RxBuffer[2]!= 0x00) {
																hm10_write_char(0x02);
																hm10_write_char(0x84);
																hm10_write_char(0x01);
																hm10_write_char(0xff);
																hm10_write_char(0x03);
													
												}else{
                        //frame doc trang thai
													valfan = TIM4 -> CCR1;
													if(valfan == 0){
														valfan = 0x00;
													}
													if(valfan == 4){
														valfan = 0x01;
													}
													if(valfan == 9){
														valfan = 0x02;
													}
													valadc = ADC_VAL[0];
																hm10_write_char(0x02);
																hm10_write_char(0x84);
																hm10_write_char(0x04);
																hm10_write_char(0x00);
																hm10_write_char(mode);
																hm10_write_char(valfan);
																hm10_write_char(valadc);
																hm10_write_char(0x03);
												}
				}
}

void USART2_IRQHandler() {

        RxBuffer[ByteInRxBuffer] = (uint8_t)(USART2 -> DR & (uint8_t) 0xFF);// lay Data tu Buffer
        ByteInRxBuffer++;
        if (RxBuffer[4] == 0x03) {
                ByteInRxBuffer = 0; // cho Byte ve 0
								RxBuffer[4] = 0;
                // check buffer
                if (RxBuffer[1] == 0x01)
								{
												if(RxBuffer[2]!= 0x01) {
																UART2_SendChar(0x02);
																UART2_SendChar(0x81);
																UART2_SendChar(0x01);
																UART2_SendChar(0xff);
																UART2_SendChar(0x03);
													
												}else{
                        if (RxBuffer[3] == 0x01) {
                                mode = 1;
																TIM4 -> CCR1 = FAN_VALUE_2;
                                UART2_SendChar(0x02);
																UART2_SendChar(0x81);
																UART2_SendChar(0x01);
																UART2_SendChar(0x00);
																UART2_SendChar(0x03);
                        } else if (RxBuffer[3] == 0x00) {
                                mode = 3; 
																TIM4 -> CCR1 = FAN_VALUE_0;
                                UART2_SendChar(0x02);
																UART2_SendChar(0x81);
																UART2_SendChar(0x01);
																UART2_SendChar(0x00);
																UART2_SendChar(0x03);
                        } else {
                                UART2_SendChar(0x02);
																UART2_SendChar(0x81);
																UART2_SendChar(0x01);
																UART2_SendChar(0x01);
																UART2_SendChar(0x03);
                        }

											}
                }
								else if (RxBuffer[1] == 0x02) 
								{
													if(RxBuffer[2]!= 0x01) {
																UART2_SendChar(0x02);
																UART2_SendChar(0x82);
																UART2_SendChar(0x01);
																UART2_SendChar(0xff);
																UART2_SendChar(0x03);
													
												}else{
																//frame chon che do
																if (RxBuffer[3] == 0x01) {
																				if (mode == 3){
																						UART2_SendChar(0x02);
																						UART2_SendChar(0x82);
																						UART2_SendChar(0x01);
																						UART2_SendChar(0x02);
																						UART2_SendChar(0x03);
																				}
																				else {
																						mode = 1;
																						UART2_SendChar(0x02);
																						UART2_SendChar(0x82);
																						UART2_SendChar(0x01);
																						UART2_SendChar(0x00);
																						UART2_SendChar(0x03);
																				}
																} else if (RxBuffer[3] == 0x00) {
																				if (mode == 3){
																						UART2_SendChar(0x02);
																						UART2_SendChar(0x82);
																						UART2_SendChar(0x01);
																						UART2_SendChar(0x02);
																						UART2_SendChar(0x03);
																				}
																				else {
																						mode = 0;
																						UART2_SendChar(0x02);
																						UART2_SendChar(0x82);
																						UART2_SendChar(0x01);
																						UART2_SendChar(0x00);
																						UART2_SendChar(0x03);
																				}
																} else {
																						UART2_SendChar(0x02);
																						UART2_SendChar(0x82);
																						UART2_SendChar(0x01);
																						UART2_SendChar(0x01);
																						UART2_SendChar(0x03);
																}
											}                      																								
              }
							else if (RxBuffer[1] == 0x03) {
								if(RxBuffer[2]!= 0x01) {
																UART2_SendChar(0x02);
																UART2_SendChar(0x83);
																UART2_SendChar(0x01);
																UART2_SendChar(0xff);
																UART2_SendChar(0x03);
													
												}else{
                        //frame chon muc gio
                        if (RxBuffer[3] == 0x00) {
                                if (mode == 3){
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x03);
																}
                                else {
                                    TIM4 -> CCR1 = FAN_VALUE_0;
                                    UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x00);
																		UART2_SendChar(0x03);
                                }
                        } else if (RxBuffer[3] == 0x01) {
                                if (mode == 3){
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x03);
																}
                                else {
                                    TIM4 -> CCR1 = FAN_VALUE_1;
                                    UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x00);
																		UART2_SendChar(0x03);
                                }
                        } else if (RxBuffer[3] == 0x02) {
                                if (mode == 3){
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x02);
																		UART2_SendChar(0x03);
																}
                                else {
                                    TIM4 -> CCR1 = FAN_VALUE_2;
                                    UART2_SendChar(0x02);
																		UART2_SendChar(0x83);
																		UART2_SendChar(0x01);
																		UART2_SendChar(0x00);
																		UART2_SendChar(0x03);
                                }
                        } else {
                                UART2_SendChar(0x02);
																UART2_SendChar(0x83);
																UART2_SendChar(0x01);
																UART2_SendChar(0x01);
																UART2_SendChar(0x03);
                        }
                     
											}
												
                }
        }
				if (RxBuffer[1] == 0x04 && RxBuffer[3] == 0x03) {
											ByteInRxBuffer = 0;
											RxBuffer[3] = 0;
											if(RxBuffer[2]!= 0x00) {
																UART2_SendChar(0x02);
																UART2_SendChar(0x84);
																UART2_SendChar(0x01);
																UART2_SendChar(0xff);
																UART2_SendChar(0x03);
													
												}else{
                        //frame doc trang thai
													valfan = TIM4 -> CCR1;
													if(valfan == 0){
														valfan = 0x00;
													}
													if(valfan == 4){
														valfan = 0x01;
													}
													if(valfan == 9){
														valfan = 0x02;
													}
													if(ADC_VAL[0]<100){
														valadc = 0x00;
													}
													if(ADC_VAL[0]>=200){
														valadc = 0xff;
													}
													if(ADC_VAL[0] < 200 && ADC_VAL[0] >= 100){
														valadc = 0x01;
													}
	
																UART2_SendChar(0x02);
																UART2_SendChar(0x84);
																UART2_SendChar(0x04);
																UART2_SendChar(0x00);
																UART2_SendChar(mode);
																UART2_SendChar(valfan);
																UART2_SendChar(valadc);
																UART2_SendChar(0x03);
												}
				}

}
