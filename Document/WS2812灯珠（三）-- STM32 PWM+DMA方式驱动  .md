# WS2812灯珠（三）-- STM32 PWM+DMA方式驱动   

[TOC]

## 一、理论

PWM输出就是对外输出脉宽（即占空比）可调的方波信号，<u>**信号频率**由自动重装载寄存器ARR的值决定，**占空比**由比较寄存器CCR的值决定。</u>

由WS2812规格书可知：

1. WS2812灯珠的通信速度为800Kbit/s。

2. 数据传输时间（TH+TL=1.25us+/-600ns）

   | T0H  | 0码，高电平时间    | 0.35us   | +/-150ns |
   | ---- | ------------------ | -------- | -------- |
   | T1H  | 1码，高电平时间    | 0.7us    | +/-150ns |
   | T0L  | 0码，低电平时间    | 0.8us    | +/-150ns |
   | T1L  | 1码，低电平时间    | 0.6us    | +/-150ns |
   | RES  | 帧单位，低电平时间 | 50us以上 |          |

故在配置STM32定时器时可设置，预分频因子为0，自动重装载初值为90-1，则频率为72M/90 = 800K。

设值占空比为**45**可满足**1码**的时间要求，设值占空比为**30**可满足**0码**的时间要求。

## 二、代码实践

- 这里在代码中具体用到的时PA0引脚（TIM2的CH1）

头文件：通过宏配置需要控制的灯珠数目

```c
#ifndef __BSP_WS281X_H
#define __BSP_WS281X_H

#include "stm32f10x.h"

#define PIXEL_NUM  16
#define GRB  24   //3*8

#define WS_HIGH 45
#define WS_LOW  30

void WS281x_Init(void);
void WS281x_CloseAll(void);
uint32_t WS281x_Color(uint8_t red, uint8_t green, uint8_t blue);
void WS281x_SetPixelColor(uint16_t n, uint32_t GRBColor);
void WS281x_SetPixelRGB(uint16_t n ,uint8_t red, uint8_t green, uint8_t blue);
void WS281x_Show(void);

void WS281x_RainbowCycle(uint8_t wait);
void WS281x_TheaterChase(uint32_t c, uint8_t wait);
void WS281x_ColorWipe(uint32_t c, uint8_t wait);
void WS281x_Rainbow(uint8_t wait);
void WS281x_TheaterChaseRainbow(uint8_t wait);
#endif /* __BSP_WS281X_H */
```

源文件：

```c
#include "bsp_ws281x.h"
#include "bsp_systick.h"

uint16_t pixelBuffer[PIXEL_NUM][GRB];

void WS281x_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
  TIM_OCInitTypeDef TIM_OCInitStruct;
  DMA_InitTypeDef DMA_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  TIM_TimeBaseInitStruct.TIM_Prescaler = 0;
  TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInitStruct.TIM_Period = 90 - 1;  //72M / 90 = 800K
  TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
  
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_Pulse = 0;
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OC1Init(TIM2, &TIM_OCInitStruct);
  TIM_CtrlPWMOutputs(TIM2,ENABLE);	//MOE 主输出使能	

  /*必须有下面该语句，否则时序会错乱*/ 
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //CH1预装载使能	 

  TIM_Cmd(TIM2, DISABLE);
  TIM_DMACmd(TIM2, TIM_DMA_CC1, ENABLE);
  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(TIM2->CCR1);
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pixelBuffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = PIXEL_NUM * GRB;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
    
  DMA_Cmd(DMA1_Channel5, DISABLE);
  
  /*关闭所有灯珠*/
  WS281x_CloseAll();
  delay_ms(100);
}

void WS281x_CloseAll(void)
{
  uint16_t i;
  uint8_t j;
  
  for(i = 0; i < PIXEL_NUM; ++i)
  {
    for(j = 0; j < 24; ++j)
    {
      pixelBuffer[i][j] = WS_LOW;
    }
  }
  WS281x_Show(); 
}


uint32_t WS281x_Color(uint8_t red, uint8_t green, uint8_t blue)
{
  return green << 16 | red << 8 | blue;
}

void WS281x_SetPixelColor(uint16_t n, uint32_t GRBColor)
{
  uint8_t i;
  if(n < PIXEL_NUM)
  {
    for(i = 0; i < GRB; i++)
    {
      pixelBuffer[n][i] = ((GRBColor << i) & 0x800000) ? WS_HIGH : WS_LOW;
    }
  }
}

void WS281x_SetPixelRGB(uint16_t n ,uint8_t red, uint8_t green, uint8_t blue)
{
  uint8_t i;
  
  if(n < PIXEL_NUM)
  {
    for(i = 0; i < GRB; ++i)
    {
      pixelBuffer[n][i] = (((WS281x_Color(red,green,blue) << i) & 0X800000) ? WS_HIGH : WS_LOW);
    }
  }
}

//下面函数中语句执行的顺序相当关键，不能错。
//从示波器看，非当前的执行顺序，最后一个灯的波形序列会多。
void WS281x_Show(void)
{
  DMA_SetCurrDataCounter(DMA1_Channel5, PIXEL_NUM * GRB);
  TIM_Cmd(TIM2, ENABLE);
  DMA_Cmd(DMA1_Channel5, ENABLE);
  while(DMA_GetFlagStatus(DMA1_FLAG_TC5) != SET);
  TIM_Cmd(TIM2, DISABLE);
  //TIM_SetCompare1(TIM2, 0); 该句不能加，加上灯不会按正常流程显示
  DMA_Cmd(DMA1_Channel5, DISABLE);
  
  DMA_ClearFlag(DMA1_FLAG_TC5);
  
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t WS281x_Wheel(uint8_t wheelPos) {
  wheelPos = 255 - wheelPos;
  if(wheelPos < 85) {
    return WS281x_Color(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  if(wheelPos < 170) {
    wheelPos -= 85;
    return WS281x_Color(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  wheelPos -= 170;
  return WS281x_Color(wheelPos * 3, 255 - wheelPos * 3, 0);
}

// Fill the dots one after the other with a color
void WS281x_ColorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<PIXEL_NUM; i++) {
    WS281x_SetPixelColor(i, c);
    WS281x_Show();
    delay_ms(wait);
  }
}

void WS281x_Rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<PIXEL_NUM; i++) {
      WS281x_SetPixelColor(i, WS281x_Wheel((i+j) & 255));
    }
    WS281x_Show();
    delay_ms(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void WS281x_RainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< PIXEL_NUM; i++) {
      WS281x_SetPixelColor(i,WS281x_Wheel(((i * 256 / PIXEL_NUM) + j) & 255));
    }
    WS281x_Show();
    delay_ms(wait);
  }
}

//Theatre-style crawling lights.
void WS281x_TheaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        WS281x_SetPixelColor(i+q, c);    //turn every third pixel on
      }
      WS281x_Show();

      delay_ms(wait);

      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        WS281x_SetPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void WS281x_TheaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        WS281x_SetPixelColor(i+q, WS281x_Wheel( (i+j) % 255));    //turn every third pixel on
      }
      WS281x_Show();

      delay_ms(wait);

      for (uint16_t i=0; i < PIXEL_NUM; i=i+3) {
        WS281x_SetPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}



```

