# STM32学习笔记(一)---基础知识

[TOC]

## 知识点

1.所有芯片的引脚顺序都是逆序排列的

2.芯片主要由内核和片上外设组成。

3.**volatile**:在 C 语言中该关键字用于修饰易变的变量，要求编译器不要优化。 

## 存储器映射

存储器本身不具有地址，给存储器分配地址的过程就称为**存储器映射**。

给存储器再分配一个地址的过程就叫做**存储器重映射**。

![存储器映射](.\图片\1565056497193.png)

## 寄存器映射

设计片上外设时，以四个字节为一个单元，共32bit，每个单元对应不同的功能，我们控制这些单元就可以驱动外设工作。

根据每个单元的功能不同，以功能为名给这个内存单元起别名，这个别名就是寄存器。

给已经分配好地址的有特定功能的内存单元取别名的过程就叫做**寄存器映射**

**例子:** GPIOF端口的输出数据寄存器ODR的内存地址为0x4002 1414（ODR寄存器32位，低16位有效，控制16个IO端口输出高低电平）

1. 通过绝对地址访问内存单元：

   ```c
    *(unsigned int *)0x40021414 = 0xFFFF;
   ```

2. 通过寄存器方式访问内存单元

   ```c
    #define GPIOF_BASE   0x40021400
    #define GPIOF_ODR   (unsigned int *)(0x40021400 + 0x14)
   
   *GPIOF_ODR = 0xFFFF;
   ```

## c语言封装寄存器

   ```c
   /* 外设基地址 */
   #define PERIPH_BASE (unsigned int)0x40000000
   
   /* 总线基地址 */
   #define APB1PERIPH_BASE PERIPH_BASE
   #define APB2PERIPH_BASE (PERIPH_BASE + 0x10000)
   #define AHB1PERIPH_BASE (PERIPH_BASE + 0x20000)
   #define AHB2PERIPH_BASE (PERIPH_BASE + 0x10000000)
   
   /* GPIO外设基地址 */
   #define GPIOA_BASE AHB1PERIPH_BASE
   #define GPIOB_BASE (AHB1PERIPH_BASE + 0x400)
   #define GPIOC_BASE (AHB1PERIPH_BASE + 0x800)
   #define GPIOD_BASE (AHB1PERIPH_BASE + 0xC00)
   #define GPIOE_BASE (AHB1PERIPH_BASE + 0x1000)
   #define GPIOF_BASE (AHB1PERIPH_BASE + 0x1400)
   #define GPIOG_BASE (AHB1PERIPH_BASE + 0x1800)
   #define GPIOH_BASE (AHB1PERIPH_BASE + 0x1C00)
   #define GPIOI_BASE (AHB1PERIPH_BASE + 0x2000)
   ```

   c语言的语法规定，结构体内变量的存储空间是连续的。

   ```c
   typedef unsigned int uint32_t; /*无符号 32 位变量*/
   typedef unsigned short int uint16_t; /*无符号 16 位变量*/
   
   /* GPIO 寄存器列表 */
   typedef struct 
   {
    uint32_t MODER; /*GPIO 模式寄存器 地址偏移: 0x00 */
    uint32_t OTYPER; /*GPIO 输出类型寄存器 地址偏移: 0x04 */
    uint32_t OSPEEDR; /*GPIO 输出速度寄存器 地址偏移: 0x08 */
    uint32_t PUPDR; /*GPIO 上拉/下拉寄存器 地址偏移: 0x0C */
    uint32_t IDR; /*GPIO 输入数据寄存器 地址偏移: 0x10 */
    uint32_t ODR; /*GPIO 输出数据寄存器 地址偏移: 0x14 */
    uint16_t BSRRL; /*GPIO 置位/复位寄存器低 16 位部分 地址偏移: 0x18 */
    uint16_t BSRRH; /*GPIO 置位/复位寄存器高 16 位部分 地址偏移: 0x1A */
    uint32_t LCKR; /*GPIO 配置锁定寄存器 地址偏移: 0x1C */
    uint32_t AFR[2]; /*GPIO 复用功能配置寄存器 地址偏移: 0x20-0x24 */
    } GPIO_TypeDef;
   ```

   ```c
   GPIO_TypeDef * GPIOx;//GPIOx为指针，存储的是变量的地址
   GPIOx = GPIOA_BASE; //将GPIOx指向GPIOA的基地址
   GPIOx->MODER = oxFFFFFFFF; //通过结构体成员访问GPIOA的MODER寄存器
   GPIOx->BSRRL = oxFFFF;
   ```


## 操作寄存器位的方法

   清零操作：&=~

   置位操作：|=

   取反操作：^=

   1. 将变量的某位清零：

   ```c
   //x&=~(1<<n)   将变量x的第n位清零
   uint8_t x = 0xff; //1111 1111 //bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
   //将变量x的第三位清零
   x &=~(1<<3); //1111 0111
   ```

   2. 将变量的某位置位：

   ```c
   //x |= (1<<n)   将变量x的第n位置位
   uint8_t x = 0xf0; //1111 0000
   //将变量x的第零位置位
   x |= (1<<0)  //1111 0001
   ```

   3. 将变量的某位取反：

   ```c
   // x ^= (1<<n)   将变量x的第n位取反
   uint8_t x = 0xff; //1111 1111
   //将变量x的第2位取反
   x ^= (1<<2); // 1111 1011
   ```

  
