#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"




void SystemInit()
{
  //ƭ��������
}
  
int main(void)
{

  /* ����GPIOA�˿�ʱ�� */
  //*RCC_AHB1ENR &=~ (1<<0);
  *RCC_AHB1ENR |= (1<<0);
  
#if 0  //ͨ�����Ե�ַ�޸ļĴ�����ֵ
  /* ���ģʽ */
  *GPIOA_MODER &=~ (0x03<<(2*6));
  *GPIOA_MODER |= (0x01<<(2*6));
  
  /* ������� */
  *GPIOA_OTYPER &=~ (1<<6);
  *GPIOA_OTYPER |= (0<<6);
  
  /* ����ٶ� */
  *GPIOA_OSPEEDR &=~ (0x03<<(2*6));
  *GPIOA_OSPEEDR |= (0x00<<(2*6));
  
  /* ������ */
  *GPIOA_PUPDR &=~ (0x03<<(2*6));
  *GPIOA_PUPDR |= (0x02<<(2*6));
   
  /* �˿�����͵�ƽ */
  //*GPIOA_ODR &=~ (0x01<<6);
  
  *GPIOA_BSRRH |= (1<<6);
  
#elif 0   //ͨ���ṹ�����ʽ���ʼĴ����������ʵ��GPIO�Ĵ����Ľṹ����ʣ�

/* ����PA7����Ϊ�������������������͵�ƽ */

  GPIOA->MODER &=~ (0x03<<(2*7));
  GPIOA->MODER |= (0x01<<(2*7));
  
  GPIOA->OTYPER &=~ (1<<7);
  GPIOA->OTYPER |= (0<<7);
  
  GPIOA->OSPEEDR &=~ (0x03<<(2*7));
  GPIOA->OSPEEDR |= (0x0<<(2*7));
  
  GPIOA->PUPDR &=~ (0x03<<(2*7));
  GPIOA->PUPDR |= (0x02<<(2*7));
  
  /*ͨ���Ĵ�������GPIO*/
  //GPIOA->BSRRH |= (1<<7);
  
  /*ͨ����װ��������GPIO*/
  //GPIO_ResetBits(GPIOA, 1<<7);
  GPIO_ResetBits(GPIOA, GPIO_Pin_7);
  
#elif 1  //���÷�װ����г�ʼ������
  
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_ResetBits(GPIOA, GPIO_Pin_7);
#endif
  while(1)
  {
    
  }
 // return 0;
}


