#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stm32f4xx.h"

void DEBUG_USART_Init(uint32_t baudrate);
void DEBUG_USART_SendByte(uint8_t byte);
uint8_t DEBUG_USART_ReceiveByte(void);

#endif /* __BSP_USART_H */

