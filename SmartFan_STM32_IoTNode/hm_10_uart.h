#ifndef HM_10_UART_H_
#define HM_10_UART_H_

#include "stdint.h"

void hm10_uart_init(uint32_t baud,uint32_t freq);

void hm10_write_char( char ch);

void hm10_write_at_command(char * ch);

void hm10_write_string(char * ch);


#endif /* HM_10_UART_H_ */
