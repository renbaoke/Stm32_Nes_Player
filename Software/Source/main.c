#include "stm32f10x.h"

int main()
{
    GPIO_InitTypeDef Led_InitData;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    Led_InitData.GPIO_Pin = GPIO_Pin_13;
    Led_InitData.GPIO_Speed = GPIO_Speed_50MHz;
    Led_InitData.GPIO_Mode = GPIO_Mode_Out_PP;

    GPIO_Init(GPIOC, &Led_InitData);

    while (1)
    {
        for (int i = 0; i < 36 * 1024 * 1024; i++);
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
        for (int i = 0; i < 36 * 1024 * 1024; i++);
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
    }
}
