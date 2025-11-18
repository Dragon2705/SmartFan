/*
 * hm_10.h
 *
 *  Created on: Jun 17, 2023
 *      Author: hussamaldean
 */

#ifndef HM_10_H_
#define HM_10_H_

#include "stdint.h"

typedef enum
{
	Buad9600=0,
	Buad19200=1,
	Baud38400=2,
	Baud57666=3,
	Baud115200=4

}HM10_BuadRate_Typedef;


typedef enum
{
	Slave=0,
	Master=1

}HM10_Role_Typedef;



void setBuadRate(HM10_BuadRate_Typedef baud);

void setRole(HM10_Role_Typedef role);

void setName(char *c);

void FactoryReset();

void moduleReset();


#endif /* HM_10_H_ */
