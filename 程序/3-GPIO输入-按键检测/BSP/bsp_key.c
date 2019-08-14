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
 * ʾ������������ʱ����1��Ӧ��ʱ��ԼΪ140ns
 */
void KEY_Delay(uint32_t time)
{
  while(time--);
}

//�������
//0���ɿ� 1������
uint8_t KEY_Check(uint16_t KEYn)
{
  if(!(GPIO_ReadInputDataBit(GPIOE, KEYn)))  //����
  {
    KEY_Delay(0x11706); //��ʱ10ms
    if(GPIO_ReadInputDataBit(GPIOE, KEYn))  //����
      return 1;
  }
  return 0;
}



