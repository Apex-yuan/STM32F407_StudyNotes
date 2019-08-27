/*** 新建工程模板 ***/

1.预定义的宏增加：USE_STDPERIPH_DRIVER,STM32F40_41xxx,HSE_VALUE=8000000

注：stm32f4xx.h文件中HSE_VALUE的值默认为25000000（即使用25M晶振），但当前开发板实际晶振使用的是8M晶振。