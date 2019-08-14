#include "bsp_usart.h"

/* 轮询方式发送接收
 * 1. 开启GPIO和USART外设时钟
 * 2. 初始化GPIO端口（TX：复用推挽输出，上拉，速度50MHz）
 * 3. 将GPIO引脚复用到USART上
 * 4. 初始化USART外设
 * 5. 使能串口
 */


void DEBUG_USART_Init(uint32_t baudrate)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
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
