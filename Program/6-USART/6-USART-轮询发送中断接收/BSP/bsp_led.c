#include "bsp_led.h"

void LED_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_SetBits(GPIOA, GPIO_Pin_6|GPIO_Pin_7); //LED OFF
}

void LED_On(uint16_t LEDn)
{
  GPIO_ResetBits(GPIOA, LEDn);
}

void LED_Off(uint16_t LEDn)
{
  GPIO_SetBits(GPIOA, LEDn);
}

void LED_Toggle(uint16_t LEDn)
{
  GPIOA->ODR ^= LEDn;
}

