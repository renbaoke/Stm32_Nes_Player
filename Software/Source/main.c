#include "stm32f10x.h"
#include "bsp_driver.h"
#include "ucos_ii.h"

extern OS_STK ShellTaskStk[SHELL_TASK_STK_SIZE];
extern void ShellTask(void *Arg);

OS_STK StartTaskStk[START_TASK_STK_SIZE];
void StartTask(void *Arg)
{
    Board_Init();
    OSStatInit();

    OSTaskCreate(ShellTask, 0, &ShellTaskStk[SHELL_TASK_STK_SIZE-1], SHELL_TASK_PRIO);

    while (1)
    {
        LED_SetStat(LED_On);
        OSTimeDlyHMSM(0, 0, 1, 0);
        LED_SetStat(LED_Off);
        OSTimeDlyHMSM(0, 0, 1, 0);
    }
}

int main()
{
    OSInit();
    OSTaskCreate(StartTask, 0, &StartTaskStk[START_TASK_STK_SIZE-1], START_TASK_PRIO);
    OSStart();
}
