#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f4xx.h"

//#define LED0_PORT  GPIOA

#define LED0 GPIO_Pin_6
#define LED1 GPIO_Pin_7
#define LED_ALL (GPIO_Pin_6 | GPIO_Pin_7)

typedef enum 
{
  OFF = 0x00,
  ON  = 0x01
}LEDState_TypeDef;

void LED_Init(void);
void LED_On(uint16_t LEDn);
void LED_Off(uint16_t LEDn);
void LED_Toggle(uint16_t LEDn);

#endif /* __BSP_LED_H */

