/*** �½��̼��⹤��ģ�� ***/

1.Ԥ����ĺ����ӣ�USE_STDPERIPH_DRIVER,STM32F40_41xxx

ע��stm32f4xx.h�ļ���HSE_VALUE��ֵĬ��Ϊ25000000����ʹ��25M���񣩣�����ǰ������ʵ�ʾ���ʹ�õ���8M����
      ����Ҫ�޸Ĺ̼���ľ������ú�ϵͳʱ�����ò��ֵ����ݣ�
     1.�޸�stm32f4xx.h�ļ�����144�� #define HSE_VALUE    ((uint32_t)25000000)  �޸�Ϊ  #define HSE_VALUE    ((uint32_t)8000000)
     2.�޸�system32_stm32f4xx.c�ļ�����371��#define PLL_M      25  �޸�Ϊ  #define PLL_M      8
ϵͳʱ�ӵ��������̣�
PLL_VCO = HSE / PLL_M * PLL_N= 8M / 8 * 336 = 336M

SYSCLK = PLL_VCO / PLL_P = 336M / 2 = 168M

HCLK = SYSCLK = 168M (AHB����ʱ��)

PCLK1 = SYSCLK / 4 = 42M (APB1����ʱ��)

PCLK2 = SYSCLK / 2 = 84M (APB2����ʱ��)
