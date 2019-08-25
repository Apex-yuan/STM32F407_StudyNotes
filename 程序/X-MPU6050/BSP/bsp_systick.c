#include "bsp_systick.h"

volatile uint32_t us_tick = 0;
volatile uint32_t ms_tick = 0;

void SYSTICK_Init(void)
{
  /* SystemCoreClock / 1000     1ms中断一次 */
  /* SystemCoreClock / 1000000  1us中断一次 */
  SysTick_Config(SystemCoreClock / 1000000); 
}

void Delay_MS(uint32_t ms)
{
  uint32_t time;
  time = ms_tick + ms;
  while(ms_tick <= time);
}

void Delay_US(uint32_t us)
{
  uint32_t time;
  time = us_tick + us;
  while(us_tick <= time);
}

