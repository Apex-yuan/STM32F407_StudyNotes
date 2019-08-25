/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bsp_led.h"
#include "bsp_systick.h"
#include "bsp_usart.h"
#include "bsp_can.h"

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

void Delay(uint32_t time)
{
  while(time--);
}

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  SYSTICK_Init();
  LED_Init();  
  DEBUG_USART_Init(115200); 
  CAN_PERIPH_Init();
  printf("\r\nStart:");
  canTxBuf.StdId = 0x0;
  canTxBuf.ExtId = 0xFFFF;
  canTxBuf.IDE = CAN_ID_EXT;
  canTxBuf.RTR = CAN_RTR_DATA;
  canTxBuf.DLC = 8;
  for(uint8_t i = 0; i < 8; ++i)
  {
    canTxBuf.Data[i] = i;
  }
  CAN_Transmit(CAN1, &canTxBuf);
  /* Wait until one of the mailboxes is empty */
  while((CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP0) !=RESET) || \
        (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP1) !=RESET) || \
        (CAN_GetFlagStatus(CANx, CAN_FLAG_RQCP2) !=RESET));
  printf("\r\n·¢ËÍÍê³É");
 
  if(canRxFlag == 1)
  {
    printf("\r\n%#x",canRxBuf.ExtId);
    printf("\r\n%d",canRxBuf.IDE);
    printf("\r\n%d",canRxBuf.RTR);
    printf("\r\n%d",canRxBuf.FMI);
    for(uint8_t i = 0; i < 8; ++i)
    {
      printf("\r\n%d",canRxBuf.Data[i]);
    }
  }
  
  /* Infinite loop */
  while (1)
  {
    // LED_Toggle(LED_ALL);
    // //Delay(0xffffff);
    // Delay_MS(500);
  }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
