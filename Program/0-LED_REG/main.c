#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"




void SystemInit()
{
  //骗过编译器
}
  
int main(void)
{

  /* 开启GPIOA端口时钟 */
  //*RCC_AHB1ENR &=~ (1<<0);
  *RCC_AHB1ENR |= (1<<0);
  
#if 0  //通过绝对地址修改寄存器的值
  /* 输出模式 */
  *GPIOA_MODER &=~ (0x03<<(2*6));
  *GPIOA_MODER |= (0x01<<(2*6));
  
  /* 推挽输出 */
  *GPIOA_OTYPER &=~ (1<<6);
  *GPIOA_OTYPER |= (0<<6);
  
  /* 输出速度 */
  *GPIOA_OSPEEDR &=~ (0x03<<(2*6));
  *GPIOA_OSPEEDR |= (0x00<<(2*6));
  
  /* 上下拉 */
  *GPIOA_PUPDR &=~ (0x03<<(2*6));
  *GPIOA_PUPDR |= (0x02<<(2*6));
   
  /* 端口输出低电平 */
  //*GPIOA_ODR &=~ (0x01<<6);
  
  *GPIOA_BSRRH |= (1<<6);
  
#elif 0   //通过结构体的形式访问寄存器（这里仅实现GPIO寄存器的结构体访问）

/* 配置PA7引脚为推挽输出，下拉，输出低电平 */

  GPIOA->MODER &=~ (0x03<<(2*7));
  GPIOA->MODER |= (0x01<<(2*7));
  
  GPIOA->OTYPER &=~ (1<<7);
  GPIOA->OTYPER |= (0<<7);
  
  GPIOA->OSPEEDR &=~ (0x03<<(2*7));
  GPIOA->OSPEEDR |= (0x0<<(2*7));
  
  GPIOA->PUPDR &=~ (0x03<<(2*7));
  GPIOA->PUPDR |= (0x02<<(2*7));
  
  /*通过寄存器操作GPIO*/
  //GPIOA->BSRRH |= (1<<7);
  
  /*通过封装函数操作GPIO*/
  //GPIO_ResetBits(GPIOA, 1<<7);
  GPIO_ResetBits(GPIOA, GPIO_Pin_7);
  
#elif 1  //利用封装库进行初始化操作
  
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


