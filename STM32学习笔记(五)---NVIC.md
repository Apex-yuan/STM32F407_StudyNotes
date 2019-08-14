# STM32学习笔记(五)---NVIC

[TOC]

F407在内核水平上搭载了一个异常响应系统，支持为数众多的系统异常和中断，其中系统异常有10个，中断有82个。

## 一、NVIC简介

1. NVIC是嵌套向量中断控制器，控制着整个芯片中断相关的功能，它跟内核紧密耦合，是内核里面的一个外设。
2. 在配置中断的时候一般只用ISER、ICER和IP这三个寄存器，ISER用来使能中断，ICER用来失能中断，IP用来设置中断的优先级。

## 二、中断优先级

### 优先级定义

NVIC有一个专门的**中断优先级寄存器NVIC_IPRx**,来配置外部中断的优先级，IPR宽度为8bit,原则上每个外部中断可配置的优先级为0-255，数值越小，优先级越高。

F407中只使用了高4位，bit[7:4]

![1565337119981](.\图片\1565337119981.png)

表达优先级的这4bit，又被分组成**抢占优先级**和**子优先级**。

1. 抢占优先级高优先执行，
2. 抢占优先级相同，子优先级高的优先执行
3. 抢占优先级和子优先级都相同，比较硬件中断编号，编号越小，优先级越高。

**别的地方看到的一个结论（未验证）：**抢占优先级高中断可以打断抢占优先级低的中断。抢占优先级相同的中断，子优先级高的不能打断子优先级低的中断。

### 优先级分组

优先级的分组由内核外设 SCB 的应用程序中断及复位控制寄存器 AIRCR 的PRIGROUP[10:8]位决定， F407 分为了 5 组。

![1565336052505](.\图片\1565336052505.png)

设置优先级分组可调用库函数NVIC_PriorityGroupConfig()实现。

```c
/**
  * @brief 配置中断优先级分组：抢占优先级和子优先级
  * @param  NVIC_PriorityGroup: 特定的优先级分组 
  *   参数如下:
  *     @arg NVIC_PriorityGroup_0: 0 bits for pre-emption priority
  *                                4 bits for subpriority
  *     @arg NVIC_PriorityGroup_1: 1 bits for pre-emption priority
  *                                3 bits for subpriority
  *     @arg NVIC_PriorityGroup_2: 2 bits for pre-emption priority
  *                                2 bits for subpriority
  *     @arg NVIC_PriorityGroup_3: 3 bits for pre-emption priority
  *                                1 bits for subpriority
  *     @arg NVIC_PriorityGroup_4: 4 bits for pre-emption priority
  *                                0 bits for subpriority
  * @note   如果优先级分组为0，则抢占优先级就不存在了，优先级就全部由子优先级控制
  * @retval None
  */
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
  /* Check the parameters */
  assert_param(IS_NVIC_PRIORITY_GROUP(NVIC_PriorityGroup));
  
  /* Set the PRIGROUP[10:8] bits according to NVIC_PriorityGroup value */
  SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup; //设置优先级分组
}
```



## 三、中断编程

1. 系统初始化时设置中断优先级分组：NVIC_PriorityGroupConfig()

2. 特定外设初始化时，使能外设的某个中断

3. 初始化NVIC_InitTypeDef结构体（配置中断优先级分组，设置抢占优先级和子优先级，使能中断请求）

   ```c
   typedef struct
   {
     uint8_t NVIC_IRQChannel;      //中断源，定义在stm32f4xx.h文件中的IRQn_Type结构体定义
     uint8_t NVIC_IRQChannelPreemptionPriority;  //抢占优先级
     uint8_t NVIC_IRQChannelSubPriority;         //子优先级
     FunctionalState NVIC_IRQChannelCmd;         //中断使能或失能
   } NVIC_InitTypeDef;
   ```

   

4. 编写中断服务函数

   **注意:**中断服务的函数名必须跟启动文件里面预先设置的一样。

