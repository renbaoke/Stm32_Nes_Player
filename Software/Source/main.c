#include "stm32f10x.h"
#include "bsp_driver.h"
#include "ucos_ii.h"
#include "ff.h"

extern OS_STK ShellTaskStk[SHELL_TASK_STK_SIZE];
extern void ShellTask(void *Arg);

extern OS_STK KeyTaskStk[KEY_TASK_STK_SIZE];
extern void KeyTask(void *Arg);

FATFS fs;
OS_EVENT *ShellMbox;

void FS_Init()
{
    f_mount(&fs, "", 0);
}

void Event_Init()
{
    ShellMbox = OSMboxCreate(0);
}

OS_STK StartTaskStk[START_TASK_STK_SIZE];
void StartTask(void *Arg)
{
    Event_Init();
    Board_Init();
    OSStatInit();
    FS_Init();

    OSTaskCreate(ShellTask, 0, &ShellTaskStk[SHELL_TASK_STK_SIZE-1], SHELL_TASK_PRIO);
    OSTaskCreate(KeyTask, 0, &KeyTaskStk[KEY_TASK_STK_SIZE-1], KEY_TASK_PRIO);

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
