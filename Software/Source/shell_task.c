#include "stm32f10x.h"
#include "bsp_driver.h"
#include "ucos_ii.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "ff.h"

int fputc(int ch, FILE *f)
{
    USART_SendData(USART1, (unsigned char)ch);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    return (ch);
}

int KeyDownPrint(uint8_t Key)
{
    printf("key %d down\r\n", Key);
    return 0;
}

int KeyUpPrint(uint8_t Key)
{
    printf("key %d up\r\n", Key);
    return 0;
}

#define CMD_BUF_SIZE        512
uint8_t CmdBuf[CMD_BUF_SIZE];

typedef int (*CmdFunc)(int Argc, char *Argv[]);
typedef struct
{
    char *name;
    CmdFunc func;
} CmdType;

int EchoCmd(int Argc, char *Argv[])
{
    if (Argc < 2)
    {
        return -1;
    }

    printf("%s", Argv[1]);

    for (uint8_t i = 2; i < Argc; i++)
    {
        printf(" %s", Argv[i]);
    }

    printf("\r\n");

    return 0;
}


int GPIOCmd(int Argc, char *Argv[])
{
    if (Argc < 4)
    {
        return -1;
    }

    GPIO_TypeDef* GPIOs[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG};

    BitAction Action;
    if (!strcmp(Argv[1], "set"))
    {
        Action = Bit_SET;
    }
    else if (!strcmp(Argv[1], "reset"))
    {
        Action = Bit_RESET;
    }
    else
    {
        return -1;
    }

    char Port = Argv[2][0];
    if (Port < 'a' || Port > 'g')
    {
        return -1;
    }

    uint16_t Pin = atoi(Argv[3]);
    GPIO_WriteBit(GPIOs[Port - 'a'], 0x1 << Pin, Action);

    return 0;
}

int CPUCmd(int Argc, char *Argv[])
{
    if (Argc < 2)
    {
        return -1;
    }

    if (!strcmp(Argv[1], "usage"))
    {
        printf("%d%%\r\n", OSCPUUsage);
    }
    else
    {
        return -1;
    }

    return 0;
}

int FlashCmd(int Argc, char *Argv[])
{
    if (Argc < 2)
    {
        return -1;
    }

    if (!strcmp(Argv[1], "id"))
    {
        printf("%x\r\n", FLASH_ManufactDeviceID());
    }
    else if (!strcmp(Argv[1], "read"))
    {
        if (Argc < 4)
        {
            return -1;
        }
        uint32_t Addr = strtoul(Argv[2], NULL, 16);
        uint16_t Size = atoi(Argv[3]);

        if (Size > CMD_BUF_SIZE)
        {
            printf("size should be less then %d\r\n", CMD_BUF_SIZE);
            return -1;
        }

        FLASH_Read(CmdBuf, Addr, Size);

        for (uint16_t i = 0; i < Size; i++)
        {
            printf("%x", CmdBuf[i]);
            if (i % 16 == 15)
            {
                printf("\r\n");
            }
        }

        printf("\r\n");
    }
    else if (!strcmp(Argv[1], "write"))
    {
        if (Argc < 4)
        {
            return -1;
        }

        uint32_t Addr = strtoul(Argv[2], NULL, 16);
        uint16_t Size = strlen(Argv[3]);

        FLASH_Write((uint8_t*)Argv[3], Addr, Size);
    }
    else if (!strcmp(Argv[1], "status"))
    {
        printf("%x\r\n", FLASH_Status());
    }
    else
    {
        return -1;
    }

    return 0;
}

int FsCmd(int Argc, char *Argv[])
{
    extern FATFS fs;

    if (Argc < 2)
    {
        return -1;
    }

    if (!strcmp(Argv[1], "ls"))
    {
        DIR dir;
        FILINFO fno;
        if (f_opendir(&dir, Argv[2]) != FR_OK)
        {
            printf("directory %s not found\r\n", Argv[1]);
            return -1;
        }

        while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0] != 0)
        {
            printf("%s\r\n", fno.fname);
        }
    }
    else
    {
        return -1;
    }

    return 0;
}

CmdType AllCmds[] =
{
    {"echo", EchoCmd},
    {"gpio", GPIOCmd},
    {"cpu", CPUCmd},
    {"flash", FlashCmd},
    {"fs", FsCmd}
};

//////////////////////////////////////////////////////////////////////////////////

#define MAX_ARGC        16
uint8_t ArgBuf[USART1_BUF_SIZE];

extern OS_EVENT *ShellMbox;

OS_STK ShellTaskStk[SHELL_TASK_STK_SIZE];
void ShellTask(void *Arg)
{
    INT8U Err;
    char *Msg;

    char*   Argv[MAX_ARGC];
    int     Argc;

    printf("\r\nwelcome to my shell\r\n");

    while(1)
    {
        Msg = OSMboxPend(ShellMbox, 0, &Err);

        Argc    = 1;
        Argv[0] = (char*)ArgBuf;

        uint16_t i = 0, j = 0;
        while (Msg[i] == ' ')
        {
            i++;
        }

        while (Msg[i] != '\0' && Argc < MAX_ARGC)
        {
            while (Msg[i] != ' ' && Msg[i] != '\0')
            {
                if (Msg[i] == '\\')
                {
                    i++;
                }
                ArgBuf[j++] = Msg[i++];
            }
            while (Msg[i] == ' ')
            {
                i++;
            }
            ArgBuf[j++] = '\0';
            Argv[Argc++] = (char*)ArgBuf + j;
        }

        Argc--;

        printf("\r\n");

        for (uint8_t i = 0; i < sizeof(AllCmds) / sizeof(CmdType); i++)
        {
            if (!strcmp(AllCmds[i].name, Argv[0]))
            {
                AllCmds[i].func(Argc, Argv);
            }
        }
    }
}
