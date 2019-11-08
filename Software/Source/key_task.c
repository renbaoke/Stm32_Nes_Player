#include "stm32f10x.h"
#include "bsp_driver.h"
#include "ucos_ii.h"

#define STAT_A 0x00
#define STAT_B 0x01
#define STAT_C 0x03
#define STAT_D 0x02

typedef int (*KeyHandler)(uint8_t Key);

extern int KeyDownPrint(uint8_t Key);
extern int KeyUpPrint(uint8_t Key);

KeyHandler KeyDownHandler[] =
{
    KeyDownPrint,
};

KeyHandler KeyUpHandler[] =
{
    KeyUpPrint,
};

void KeyDown(uint8_t Key)
{
    for (uint8_t i = 0; i < sizeof(KeyDownHandler) / sizeof(KeyHandler); i++)
    {
        if (KeyDownHandler[i](Key))
        {
            break;
        }
    }
}

void KeyUp(uint8_t Key)
{
    for (uint8_t i = 0; i < sizeof(KeyUpHandler) / sizeof(KeyHandler); i++)
    {
        if (KeyUpHandler[i](Key))
        {
            break;
        }
    }
}

uint32_t KeyStatus = 0;

void GotoStat_A(uint8_t Key)
{
    KeyStatus &= ~(0x3 << (Key * 2));
}

void GotoStat_B(uint8_t Key)
{
    KeyStatus &= ~(0x2 << (Key * 2));
    KeyStatus |= 0x1 << (Key * 2);
}

void GotoStat_C(uint8_t Key)
{
    KeyStatus |= 0x3 << (Key * 2);
}

void GotoStat_D(uint8_t Key)
{
    KeyStatus |= 0x2 << (Key * 2);
    KeyStatus &= ~(0x1 << (Key * 2));
}

OS_STK KeyTaskStk[KEY_TASK_STK_SIZE];
void KeyTask(void *Arg)
{
    uint16_t InputKeys = 0;

    while (1)
    {
        OSTimeDlyHMSM(0, 0, 0, 20);

        InputKeys = 0;

        for (uint8_t i = 0; i < 4; i++)
        {
            KEY_SetRowStat(i, Key_Set);
            for (uint8_t j = 0; j < 4; j++)
            {
                if (Key_GetColStat(j) == Key_Set)
                {
                    InputKeys |= (0x1 << ((i << 2 ) + j));
                }
            }
            KEY_SetRowStat(i, Key_Reset);
        }

        for (uint8_t i = 0; i < 16; i++)
        {
            switch ((KeyStatus >> (i * 2)) & 0x3)
            {
            case STAT_A:
                if (InputKeys & (0x1 << i))
                {
                    GotoStat_B(i);
                }
                break;
            case STAT_B:
                if (InputKeys & (0x1 << i))
                {
                    GotoStat_C(i);
                    KeyDown(i);
                }
                else
                {
                    GotoStat_A(i);
                }
                break;
            case STAT_C:
                if (!(InputKeys & (0x1 << i)))
                {
                    GotoStat_D(i);
                }
            case STAT_D:
                if (InputKeys & (0x1 << i))
                {
                    GotoStat_C(i);
                }
                else
                {
                    GotoStat_A(i);
                    KeyUp(i);
                }
                break;
            }
        }
    }
}
