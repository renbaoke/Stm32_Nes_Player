#ifndef __BSP_DRIVER_H__
#define __BSP_DRIVER_H__

#include "stm32f10x.h"

#define LED_ON          1
#define LED_OFF         0

void SysTick_Init(void);

void LED_Init(void);
void LED_SetStat(uint8_t stat);

#endif
