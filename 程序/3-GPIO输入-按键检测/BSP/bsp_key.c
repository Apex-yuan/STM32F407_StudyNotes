#include "bsp_key.h"

void KEY_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOE, &GPIO_InitStruct);
  
  GPIO_SetBits(GPIOE, GPIO_Pin_3 | GPIO_Pin_4);
}

/*
 * 示波器测量：延时计数1对应的时间约为140ns
 */
void KEY_Delay(uint32_t time)
{
  while(time--);
}

//按键检测
//0：松开 1：按下
uint8_t KEY_Check(uint16_t KEYn)
{
  if(!(GPIO_ReadInputDataBit(GPIOE, KEYn)))  //按下
  {
    KEY_Delay(0x11706); //延时10ms
    if(GPIO_ReadInputDataBit(GPIOE, KEYn))  //弹起
      return 1;
  }
  return 0;
}



