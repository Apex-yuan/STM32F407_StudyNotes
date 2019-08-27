# STM32学习笔记(三)---启动文件

[TOC]

## 启动文件简介

启动文件由汇编编写，是系统上电复位后第一个执行的程序。

1. 初始化堆栈指针 SP=_initial_sp
2. 初始化PC指针=Reset_Handler
3. 初始化中断向量表
4. 配置系统时钟
5. 调用C库函数_main初始化用户堆栈，最终调用main函数去到C的世界

## 启动文件中用到的ARM汇编指令

| 名称          | 作用                                                    |
| ------------- | ------------------------------------------------------- |
| EQU           | 给数字常量取一个符号名                                  |
| AREA          | 汇编一个新的代码段或数据段                              |
| SPACE         | 分配内存空间                                            |
| PRESERVE8     | 当前文件堆栈需要按照8字节对齐                           |
| IMPORT        | 声明标号来自外部文件                                    |
| EXPORT        | 声明一个标号具有全局属性，可被外部的文件使用            |
| DCD           | 以字为单位分配内存，要求4字节对齐，并要求初始化这些内存 |
| PROC          | 定义子程序，与**ENDP**成对使用，表示子程序结束          |
| B             | 跳转到一个标号                                          |
| END           | 到达文件末尾，文件结束                                  |
| IF,ELSE,ENDIF | 汇编条件分支语句                                        |

## 启动文件分析

### 1. 栈

```asm
Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp
```

开辟栈空间大小为0x00000400(1kb)，名字为STACK，NOINIT表示不初始化，可读可写，8（2^3）字节对齐。

标号__initial_sp紧挨着SPACE语句放置，表示栈的结束地址，即栈顶指针，<u>栈是由高向低生长的</u>。

**栈作用：**用于局部变量，函数调用，函数形参等的开销。

### 2. 堆

 ```assembly
Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit
 ```

开辟堆空间的大小为0x00000200(512byte)，名字为HEAP，NOINIT表示不初始化，可读可写，8(2^3)字节对齐。

**__heap_base**表示堆的起始地址，**__heap_limit**表示堆的结束地址。<u>堆是由低向高生长的</u>

**堆作用：**主要用来动态内存的分配（一般是由malloc()函数申请的内存）。

### 3. 向量表

```assembly
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
```

汇编一个新的数据段，名字为**RESET**，可读。声明**__Vectors**、**__Vectors_End**和**__Vectors_Size**这三个标号具有全局属性，可供外部调用。

```assembly
__Vectors       DCD     __initial_sp               ; Top of Stack  ;栈顶指针
                DCD     Reset_Handler              ; Reset Handler ;复位程序地址
                DCD     NMI_Handler                ; NMI Handler
                DCD     HardFault_Handler          ; Hard Fault Handler
                DCD     MemManage_Handler          ; MPU Fault Handler
                DCD     BusFault_Handler           ; Bus Fault Handler
                DCD     UsageFault_Handler         ; Usage Fault Handler
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     SVC_Handler                ; SVCall Handler
                DCD     DebugMon_Handler           ; Debug Monitor Handler
                DCD     0                          ; Reserved
                DCD     PendSV_Handler             ; PendSV Handler
                DCD     SysTick_Handler            ; SysTick Handler

                ; External Interrupts
                DCD     WWDG_IRQHandler                   ; Window WatchDog     
                DCD     PVD_IRQHandler                                          
                DCD     TAMP_STAMP_IRQHandler                       
                ;中间省略部分代码                           
                DCD     CRYP_IRQHandler                   ; CRYP crypto    
                DCD     HASH_RNG_IRQHandler               ; Hash and Rng
                DCD     FPU_IRQHandler                    ; FPU
                                         
__Vectors_End

__Vectors_Size  EQU  __Vectors_End - __Vectors
```

**__Vectors**为向量表起始地址，**__Vectors_End**为向量表结束地址。

向量表从FLASH的0地址开始存放，以4个字节为一个单位，地址0存放的是栈顶指针，0x04存放的是复位程序的地址…… 

在向量表中，DCD分配了一堆内存，并且以**异常服务例程(ESR)**的入口地址初始化他们。

### 4. 复位程序

```assembly
                AREA    |.text|, CODE, READONLY

; Reset handler
Reset_Handler    PROC
                 EXPORT  Reset_Handler             [WEAK]
        IMPORT  SystemInit
        IMPORT  __main

                 LDR     R0, =SystemInit
                 BLX     R0
                 LDR     R0, =__main
                 BX      R0
                 ENDP
```

定义一个名为.text的代码段，可读。

**复位子程序是上电后第一个执行的程序**：调用SystemInit()函数初始化系统时钟，然后调用C库函数__main，最终调用main函数去到C的世界。

| 内核指令名称 | 作用                                                         |
| :----------- | ------------------------------------------------------------ |
| LDR          | 从存储器中加载字到一个寄存器中                               |
| BL           | 跳转到由寄存器/标号给出的地址，并把跳转前的下条指令地址保存到LR |
| BLX          | 跳转到由寄存器给出的地址，并根据寄存器的LSE确定处理器的状态，还要把跳转前的下条指令地址保存到LR |
| BX           | 跳转到由寄存器/标号给出的地址，不用返回                      |

### 5.中断服务程序

启动文件中已经写好了所有的中断的中断服务函数，这写中断服务函数都为空函数，真正的中断服务函数需要我们在外部的C文件里面重新实现。

```assembly
NMI_Handler     PROC
                EXPORT  NMI_Handler                [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler          [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler          [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler           [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler           [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler             [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler            [WEAK]
                B       .
                ENDP

Default_Handler PROC

                EXPORT  WWDG_IRQHandler                   [WEAK]                                        
                EXPORT  PVD_IRQHandler                    [WEAK]                      
                EXPORT  TAMP_STAMP_IRQHandler             [WEAK]         
;省略中间部分代码                                                          
CRYP_IRQHandler                                                    
HASH_RNG_IRQHandler
FPU_IRQHandler
   
                B       .

                ENDP
```

B：跳转到一个标号。<u>'.'表示无限循环</u>

**注意：**在使用某个外设的时候，开启了某个中断，但是没有写中断服务函数或函数名称写错，中断来临时就会跳转到这些预先写好的空的中断服务函数中，并卡死在这里。

### 6. 用户堆栈初始化

```assembly
                IF      :DEF:__MICROLIB
                
                 EXPORT  __initial_sp
                 EXPORT  __heap_base
                 EXPORT  __heap_limit
                
                 ELSE
                
                 IMPORT  __use_two_region_memory
                 EXPORT  __user_initial_stackheap
                 
__user_initial_stackheap

                 LDR     R0, =  Heap_Mem
                 LDR     R1, =(Stack_Mem + Stack_Size)
                 LDR     R2, = (Heap_Mem +  Heap_Size)
                 LDR     R3, = Stack_Mem
                 BX      LR

                 ALIGN

                 ENDIF

                 END
```

根据是否使用**微库**来执行不同的初始化配置。**用户堆栈的初始化，由C库函数__main来完成。**

