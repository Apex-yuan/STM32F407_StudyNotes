/*** 新建固件库工程模板 ***/

1.预定义的宏增加：USE_STDPERIPH_DRIVER,STM32F40_41xxx

注：stm32f4xx.h文件中HSE_VALUE的值默认为25000000（即使用25M晶振），但当前开发板实际晶振使用的是8M晶振。
      故需要修改固件库的晶振配置和系统时钟配置部分的内容：
     1.修改stm32f4xx.h文件：将144行 #define HSE_VALUE    ((uint32_t)25000000)  修改为  #define HSE_VALUE    ((uint32_t)8000000)
     2.修改system32_stm32f4xx.c文件：将371行#define PLL_M      25  修改为  #define PLL_M      8
系统时钟的配置流程：
PLL_VCO = HSE / PLL_M * PLL_N= 8M / 8 * 336 = 336M

SYSCLK = PLL_VCO / PLL_P = 336M / 2 = 168M

HCLK = SYSCLK = 168M (AHB总线时钟)

PCLK1 = SYSCLK / 4 = 42M (APB1总线时钟)

PCLK2 = SYSCLK / 2 = 84M (APB2总线时钟)
