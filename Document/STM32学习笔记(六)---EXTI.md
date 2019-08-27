# STM32学习笔记(六)---EXTI

[TOC]

EXTI: 外部中断/事件控制器

## 一、EXTI简介

1. 外部中断/事件控制器包含多大23个用于产生事件/中断请求的边沿检测器。
2. 每根输入线都可单独进行配置：选择类型（中断/事件）和相应的触发事件（上升沿、下降沿或边沿触发）
3. 每根输入线可单独屏蔽。

**注意：**检测脉冲宽度低于APB2时钟宽度的外部信号

## 二、EXTI框图

![1565328850314](.\图片\1565328850314.png)

EXITI可分为两大功能：产生中断和产生事件

### 1. 产生中断

红色虚线指示的流程

编号1：输入线可以通过寄存器设置为任意一个GPIO，也可以是一些外设的事件。

编号2：边沿检测电路检测到EXTI_RTSR(上边沿寄存器)或EXTI_FTSR(下边沿寄存器)有效信号就输出1无效信号输出0

编号3：或门电路，EXTI_SWIER(软件中断/事件寄存器)允许我们通过程序控制就可以启动中断/事件线

编号4：与门电路，只有两侧输入都有效时才有效，因此EXTI_IMR(中断屏蔽寄存器)可以控制是否产生中断

编号5：将EXTI_PR（挂起请求寄存器）的内容输出到NVIC内，从而实现系统中断事件控制。

**产生中断线路的目的**是把输入信号输入到NVIC，进一步会运行中断服务函数，实现功能。（软件级）

### 2. 产生事件

绿色虚线指示的流程

编号6：与门电路，可以通过EXTI_EMR来实现是否产生事件。

编号7：脉冲发生电路，输入有效信号就会产生脉冲

编号8：脉冲信号，产生事件线路的最终产物，这些信号供其他外设电路使用，如：定时器TIM的输入捕获功能，编码器功能等。

**产生事件线路的目的**就是传输一个脉冲信号给其他外设使用，并且是电路级别的信号传输。（硬件级）

## 外部中断/事件线映射

EXTI有23个中断/事件线，每个GPIO都可以被设置为输入线，占用EXTI0至EXTI15,还有七根用于特定的外设事件。

![1565333567860](.\图片\1565333567860.png)

EXTI0可以通过SYSCFG外部中断配置寄存器1（SYSCFG_EXTICR1）的EXTI[3:0]选择配置为PA0、PB0......PI0。

**注意：**<u>PA0和PB0无法同时绑定到EXTI0中断线上，在制作硬件电路和编程时要注意</u>

## 编程流程

以按键(PE4)中断为例：

1. 开启GPIO时钟和SYSCFG时钟：(**GPIO外部中断连接由SYSCFG控制**)

   ```c
   RCC_AHB1PeriphClkCmd(RCC_AHB1Periph_GPIOE, ENABLE);  //
   RCC_APB2PeriphClkCmd(RCC_APB2Periph_SYSCFG, ENABLE); //SYSCFG管理GPIO的外部中断连接
   ```

2. 初始化外部中断的GPIO端口配置：

   ```c
   GPIO_InitTypeDef GPIO_InitStruct;
   
   GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
   GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_Init(GPIOE, &GPIO_InitStruct);
   ```

3. 将GPIO连接到EXTI输入线

   ```c
   SYSCFG_EXTILineConfig(EXTI_PortLineSourceGPIOE, EXTI_PinSource4);
   ```

4. 初始化EXTI配置

   ```c
   EXTI_InitTypeDef EXTI_InitStruct;
   
   EXTI_InitStruct.EXTI_Line = EXTI_Line4;             //中断线4
   EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;    //中断模式
   EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发
   EXTI_InitStruct.EXTI_LineCmd = ENABLE;              //中断线使能
   EXTI_Init(&EXTI_InitStruct);
   ```

5. 配置中断优先级分组

   ```c
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
   ```

6. 初始化NVIC

   ```c
   NVIC_InitTypeDef NVIC_InitStruct;
   
   NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
   NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
   NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStruct);
   ```

7. 编写中断服务函数

   ```c
   void EXTI4_IRQHandler(void)
   {
     if(EXTI_GetITStatus(EXTI_Line4) != RESET)
     {
       EXTI_ClearITPendingBit(EXTI_Line4);
       
       EXTI_Delay((uint32_t)71430);  //延时10ms
       LED_Toggle(LED0);
     }
   }
   ```

   

