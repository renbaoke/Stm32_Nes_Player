#include "stm32f10x.h"
#include "bsp_driver.h"
#include "ucos_ii.h"

void SysTick_Init()
{
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);
    SysTick_Config(rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC);
}

void LED_Init()
{
    GPIO_InitTypeDef Led_InitData;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    Led_InitData.GPIO_Pin = GPIO_Pin_13;
    Led_InitData.GPIO_Speed = GPIO_Speed_50MHz;
    Led_InitData.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_Init(GPIOC, &Led_InitData);
}

void LED_SetStat(uint8_t stat)
{
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, stat ? Bit_RESET : Bit_SET);
}
