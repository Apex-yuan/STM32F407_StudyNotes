#include "bsp_can.h"

CanTxMsg canTxBuf;
CanRxMsg canRxBuf;
uint8_t canRxFlag = 0;

void CAN_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  CAN_GPIO_CLK_FUNCTION(CAN_GPIO_CLK, ENABLE);
  
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
  GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT);
  
  GPIO_InitStruct.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStruct);
}

void CAN_MODE_Config(void)
{
  CAN_InitTypeDef CAN_InitStruct;
  
  CAN_CLK_FUNCTION(CAN_CLK, ENABLE);
  
  CAN_DeInit(CANx);
  
  CAN_InitStruct.CAN_TTCM = DISABLE;
  CAN_InitStruct.CAN_ABOM = ENABLE;
  CAN_InitStruct.CAN_AWUM = ENABLE;
  CAN_InitStruct.CAN_NART = ENABLE;
  CAN_InitStruct.CAN_RFLM = ENABLE;
  CAN_InitStruct.CAN_TXFP = ENABLE;
  CAN_InitStruct.CAN_Mode = CAN_Mode_LoopBack;
  CAN_InitStruct.CAN_SJW = CAN_SJW_2tq;
  /* CAN Baudrate = 1MBps */
  CAN_InitStruct.CAN_Prescaler = 6;
  CAN_InitStruct.CAN_BS1 = CAN_BS1_4tq;
  CAN_InitStruct.CAN_BS2 = CAN_BS2_2tq;
  CAN_Init(CANx, &CAN_InitStruct);
}

void CAN_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStruct;
  
  NVIC_InitStruct.NVIC_IRQChannel = CAN_RX_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStruct);
}

void CAN_FILTER_Config(void)
{
  CAN_FilterInitTypeDef CAN_FilterInitStruct;
  
  CAN_FilterInitStruct.CAN_FilterNumber = 0;
  CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_32bit;
  CAN_FilterInitStruct.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStruct.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStruct.CAN_FilterFIFOAssignment = CAN_FilterFIFO0;
  CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStruct);
}

void CAN_PERIPH_Init(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_MODE_Config();
  CAN_FILTER_Config();
  
  /* 使能CAN FIFO 0 接收中断 */
  CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}

void CAN1_RX0_IRQHandler(void)
{
  if(CAN_GetITStatus(CANx, CAN_IT_FMP0) != RESET)
  {
    /* 设置了接收报文中断，必须在中断中调用该函数，只有这样才能调用清除接收中断的标志。 */
    CAN_Receive(CANx, CAN_FIFO0, &canRxBuf);
    canRxFlag = 1;
  }
}  


