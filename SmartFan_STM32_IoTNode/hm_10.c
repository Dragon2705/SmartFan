/*
 * hm_10.c
 *
 *  Created on: Jun 17, 2023
 *      Author: hussamaldean
 */


#include "hm_10.h"
#include "hm_10_uart.h"
#include "stdio.h"
#include "stm32f4xx.h"
#include "led.h"


static char AT_Array[20];



void setBuadRate(HM10_BuadRate_Typedef baud)
{

	sprintf(AT_Array,"AT+BAUD%d\r\n",baud);
	hm10_write_at_command(AT_Array);
}

void setRole(HM10_Role_Typedef role)
{
	sprintf(AT_Array,"AT+ROLE%d\r\n",role);
	hm10_write_at_command(AT_Array);
}

void setName(char *c)
{
	sprintf(AT_Array,"AT+NAME%s\r\n",c);
	hm10_write_at_command(AT_Array);
}

void FactoryReset()
{
	sprintf(AT_Array,"AT+RENEW\r\n");
	hm10_write_at_command(AT_Array);
}

void moduleReset()
{
	sprintf(AT_Array,"AT+RESET\r\n");
	hm10_write_at_command(AT_Array);
}





