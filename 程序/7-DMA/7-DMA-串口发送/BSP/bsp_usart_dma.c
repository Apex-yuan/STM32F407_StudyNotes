#include "bsp_usart_dma.h"

/* 串口DMA发送
 * 1. 开启GPIO和USART外设时钟
 * 2. 初始化GPIO端口（TX：复用推挽输出，上拉，速度50MHz）
 * 3. 将GPIO引脚复用到USART上
 * 4. 初始化USART外设
 * 5. 使能串口
 */

uint8_t buffer[4096];

void USART_DMA_Init(uint32_t baudrate)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  DMA_InitTypeDef DMA_InitStruct;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10,GPIO_AF_USART1);
  
  USART_InitStruct.USART_BaudRate = (uint32_t)baudrate;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_Parity = USART_Parity_No;
  USART_InitStruct.USART_StopBits = USART_StopBits_1;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(USART1, &USART_InitStruct);

  DMA_DeInit(DMA2_Stream7);
  DMA_InitStruct.DMA_Channel = DMA_Channel_4;
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t )&(USART1->DR);
  DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t )buffer;
  DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_InitStruct.DMA_BufferSize = 4096;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
  DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream7, &DMA_InitStruct);
  
  DMA_Cmd(DMA2_Stream7, ENABLE);
  
  USART_Cmd(USART1, ENABLE);
}

void DEBUG_USART_SendByte(uint8_t byte)
{
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  USART_SendData(USART1, byte);
}

uint8_t DEBUG_USART_ReceiveByte(void)
{
  if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
  {
    return USART_ReceiveData(USART1);
  }
  else
  {
    return 0;
  }

}
