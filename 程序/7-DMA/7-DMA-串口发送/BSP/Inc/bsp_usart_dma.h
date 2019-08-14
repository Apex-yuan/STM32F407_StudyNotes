#ifndef __BSP_USART_DMA_H
#define __BSP_USART_DMA_H

#include "stm32f4xx.h"

extern uint8_t buffer[];

void USART_DMA_Init(uint32_t baudrate);
void DEBUG_USART_SendByte(uint8_t byte);
uint8_t DEBUG_USART_ReceiveByte(void);

#endif /* __BSP_USART_DMA_H */

