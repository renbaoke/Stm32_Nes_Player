#include "stm32f10x.h"
#include "bsp_driver.h"
#include "ucos_ii.h"

OS_STK start_stk[START_TASK_STK_SIZE];

void start_task(void *pdata)
{
    SysTick_Init();
    LED_Init();

    while (1)
    {
        LED_SetStat(LED_ON);
        OSTimeDlyHMSM(0, 0, 1, 0);
        LED_SetStat(LED_OFF);
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

int main()
{
    OSInit();
    OSTaskCreate(start_task, 0, &start_stk[START_TASK_STK_SIZE-1], START_TASK_PRIO);
    OSStart();
}
