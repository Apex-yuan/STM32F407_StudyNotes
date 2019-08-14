#ifndef __STM32F4XX_H
#define __STM32F4XX_H

//#ifdef __cplusplus
//extern "C" {
//#endif

/* 外设基地址 */
#define PERIPH_BASE  ((unsigned int)0x40000000)
  
/* 总线基地址 */
#define APB1_BASE PERIPH_BASE
#define APB2_BASE (PERIPH_BASE + 0x10000)
#define AHB1_BASE (PERIPH_BASE + 0x20000)
#define AHB2_BASE (PERIPH_BASE + 0x10000000)

/* GPIO外设基地址 */
#define GPIOA_BASE AHB1_BASE
#define GPIOB_BASE (AHB1_BASE + 0x400)
#define GPIOC_BASE (AHB1_BASE + 0x800)
#define GPIOD_BASE (AHB1_BASE + 0xC00)
#define GPIOE_BASE (AHB1_BASE + 0x1000)
#define GPIOF_BASE (AHB1_BASE + 0x1400)
#define GPIOG_BASE (AHB1_BASE + 0x1800)

/* RCC外设基地址 */
#define RCC_BASE      (AHB1_BASE + 0x3800)


/**************使用寄存器绝对地址*********************/

/* RCC寄存器地址 */
#define RCC_AHB1ENR   (unsigned int *)(RCC_BASE + 0x30)

/* GPIOA寄存器地址 */
#define GPIOA_MODER   (unsigned int *)GPIOA_BASE
#define GPIOA_OTYPER  (unsigned int *)(GPIOA_BASE + 0x04)
#define GPIOA_OSPEEDR (unsigned int *)(GPIOA_BASE + 0x08)
#define GPIOA_PUPDR   (unsigned int *)(GPIOA_BASE + 0x0C)
#define GPIOA_IDR     (unsigned int *)(GPIOA_BASE + 0x10)
#define GPIOA_ODR     (unsigned int *)(GPIOA_BASE + 0x14)
#define GPIOA_BSRR    (unsigned int *)(GPIOA_BASE + 0x18)
#define GPIOA_LCKR    (unsigned int *)(GPIOA_BASE + 0x1C)
#define GPIOA_AFRL    (unsigned int *)(GPIOA_BASE + 0x20)
#define GPIOA_AFRH    (unsigned int *)(GPIOA_BASE + 0x24)

/****************利用C语言结构体封装寄存器*********************/
#define __IO  volatile

typedef unsigned int  uint32_t;
typedef unsigned short uint16_t;

typedef struct
{
  __IO uint32_t MODER;    //0x0
  __IO uint32_t OTYPER;   //0x04
  __IO uint32_t OSPEEDR;  //0x08
  __IO uint32_t PUPDR;    //0x0C
  __IO uint32_t IDR;      //0x10
  __IO uint32_t ODR;      //0x14   
  __IO uint16_t BSRRL;     //0x18
  __IO uint16_t BSRRH;     //0x1A 
  __IO uint32_t LCKR;     //0x1C
  __IO uint32_t AFRL;     //0x20
  __IO uint32_t AFRH;     //0x24
}GPIO_TypeDef;

//将GPIOA_BASE强制转化为GPIO_TypeDef类型的指针（地址），则GPIOA_BASE地址存储的即是GPIO_TypeDef结构体类型的变量。
//该结构体变量存储的首地址即为GPIOA_BASE地址
//根据c语言特性，结构体变量是一段连续的空间，因此可以根据起始地址和对应的地址偏移访问对应的寄存器。
#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE) 
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE ((GPIO_TypeDef *)GPIOE_BASE)
#define GPIOF ((GPIO_TypeDef *)GPIOF_BASE)


/***************进一步封装实现固件库的基本雏形**************************/
//新建立stm32f4xx_gpio.c/h文件来实现对GPIO的进一步封装。

//#ifdef __cplusplus
// }
//#endif

#endif /* __STM32F4XX_H */

