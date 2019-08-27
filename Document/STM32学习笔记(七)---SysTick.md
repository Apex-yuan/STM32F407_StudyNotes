# STM32学习笔记(七)---SysTick

[TOC]

## 一、SysTick简介

1. SysTick属于CM4内核中的一个外设，内嵌在NVIC中，<u>有关寄存器的定义和部分库函数都在core_cm4.h头文件中实现</u>。

2. SysTick(系统滴答定时器)是一个**24bit**的**向下递减**的计数器，计数器每计数一次的时间为**1/SYSCLK**。

   当重装载初值寄存器的值减到0的时候，系统定时器就产生一次中断，一次循环往复。

3. 系统定时器一般用于操作系统，用于产生时基，维持操作系统心跳。

## 二、SysTick功能框图

![功能框图](.\图片\1565322107275.png)

## 三、SysTick寄存器

| 寄存器名称 | 寄存器描述       |
| ---------- | ---------------- |
| CTRL       | 控制及状态寄存器 |
| LOAD       | 重装载数值寄存器 |
| VAL        | 当前数值寄存器   |
| CALIB      | 校准数值寄存器   |

SysTick控制和状态寄存器

| 位   | 名称      | 类型 | 复位值 | 描述                                                         |
| ---- | --------- | ---- | ------ | ------------------------------------------------------------ |
| 16   | COUNTFLAG | RO   | 0      | 当SysTick定时器计数到0时，该位变为1，读取寄存器或清除计数器当前值会被清零 |
| 2    | CLKSOURCE | R/W  | 0      | 时钟选择位：0=SYSCLK/8, 1=SYSCLK                             |
| 1    | TICKINT   | R/W  | 0      | 1 = SYSTICK定时器计数减至0时产生异常                               0 = 不产生异常 |
| 0    | ENABLE    | R/W  | 0      | SYSTICK定时器使能                                            |

SysTick重装载值寄存器

| 位    | 名称   | 类型 | 复位值 | 描述                      |
| ----- | ------ | ---- | ------ | ------------------------- |
| 23：0 | RELOAD | R/W  | 未定义 | 定时器计数为0时的重装载值 |

SysTick当前值寄存器

| 位    | 名称    | 类型 | 复位值 |                                                              |
| ----- | ------- | ---- | ------ | ------------------------------------------------------------ |
| 23：0 | CURRENT | R/Wc | 0      | 读出值为SYSTICK定时器的当前值。写入任何值都会清除寄存器，SYSTICK控制和状态寄存器中的COUNTFLAG也会清零 |

## 四、SYSTICK使用

### 1.中断方式使用

可以直接通过库函数SysTick_Config(uint32_t ticks)来配置，该函数位于core_m4.h文件中

```c
__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)
{
    /* 重装载初值超出范围 */
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk) { return (1UL); }    
  /* 设置重装载初值 */
  SysTick->LOAD  = (uint32_t)(ticks - 1UL);              
  /* 设置SysTick定时器中断优先级，这里将SysTick的优先级设置位15（0-15），为系统最低 */
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); 
  /* 设置当前数值寄存器 */
  SysTick->VAL   = 0UL;
  /* 设置时钟源的选择：SYSCLK=168M 
   * 使能系统定时器中断
   * 使能定时器
   */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         
  return (0UL);                                                   
}
```

```c
 /* SystemCoreClock / 1000      1ms中断一次
 * SystemCoreClock / 1000000   1us中断一次
 */
Systick_Config(SystemCoreClock / 1000);
```

**注意：**这里的tick值最大为**2^24** = 16,777,216‬,不能超过此值。因此上面函数不能够配置1s中断（1s中断tick值为：168000000>16777216）。

```c
void SysTick_Handler(void)
{
  /* 根据自己的实际需求来写 */    
}
```



### 2.轮询方式使用

若要在轮询模式中使用SysTick定时器，则需要通过上述寄存器来配置。可以利用SysTick控制和状态寄存器的（SysTick->CTRL）中的计数标志来确定定时器何时变为0。

```c
Systick->CTRL = 0;                            //禁止SysTick
SysTick->LOAD = 0xFF;                         //计数范围255-0（256个周期）
SysTick->VAL = 0;                             //清除当前值和计数标记
SysTick->CTRL = 5;                            //使能SysTick定时器并使用处理器时钟
while((SysTick->CRTL & 0x00010000) == 0);     //等待计数标志置位
SysTick->CTRL = 0;                            //禁止SysTick
```

上面这段代码便实现了延时265个时钟周期的延时功能，具体的延时函数可以参考正点原子的延时函数。

------

上述两种实现方法都是在**裸机条件**下的使用方法，在实际应用时可以根据需求做选择。

## 五、参考资料：

1.《零死角玩转STM32-F407霸天虎》（SysTick）

2.《STM32F4xx标准外设库源码》（core_cm4.h）

3.《ARM Cortex-M3与Cortex-M4权威指南》（9.5 SysTick定时器）