#ifndef __BSP_DRIVER_H__
#define __BSP_DRIVER_H__

#include "stm32f10x.h"

typedef struct
{
    GPIO_TypeDef* GPIO_Port;
    uint16_t GPIO_Pin;
    GPIOSpeed_TypeDef GPIO_Speed;
    GPIOMode_TypeDef GPIO_Mode;
} GPIO_InitDef;

typedef enum
{
    LED_On = 0,
    LED_Off
} LED_Stat;

#define GPIO_INIT(x) GPIO_Init((x).GPIO_Port, (GPIO_InitTypeDef*)&(x).GPIO_Pin)

#define USART1_BUF_SIZE         64

void SysTick_Init(void);

void LED_Init(void);
void LED_SetStat(LED_Stat Stat);

void USART1_Init(void);

void Board_Init(void);

#endif
