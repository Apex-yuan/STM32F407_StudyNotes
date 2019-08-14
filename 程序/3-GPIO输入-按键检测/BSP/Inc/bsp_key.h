#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#include "stm32f4xx.h"


#define KEY0 GPIO_Pin_4
#define KEY1 GPIO_Pin_3


typedef enum 
{
  UP = 0x00,     //�ɿ�
  DOWN  = 0x01,  //�̰�
  LDOWN = 0x02   //����
}KEYState_TypeDef;

void KEY_Init(void);
uint8_t KEY_Check(uint16_t KEYn);

#endif /* __BSP_KEY_H */

