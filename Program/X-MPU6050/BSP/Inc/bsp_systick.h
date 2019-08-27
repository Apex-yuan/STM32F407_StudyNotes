#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H

#include "stm32f4xx.h"

extern volatile uint32_t us_tick;
extern volatile uint32_t ms_tick;

void SYSTICK_Init(void);
void Delay_MS(uint32_t ms);
void Delay_US(uint32_t us);

#endif /* __BSP_SYSTICK_H */

