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

typedef enum
{
    LCD_On = 0,
    LCD_Off
} LCD_Stat;

typedef enum
{
    Key_Reset = 0,
    Key_Set
} Key_Stat;

#define GPIO_INIT(x)            GPIO_Init((x).GPIO_Port, (GPIO_InitTypeDef*)&(x).GPIO_Pin)

#define USART1_BUF_SIZE         64

#define LCD_WIDTH               320
#define LCD_HEIGHT              240
#define COLOR(x)                ((((x) & 0x00ff) << 8) | (((x) & 0xff00) >> 8))

void SysTick_Init(void);

void LED_Init(void);
void LED_SetStat(LED_Stat Stat);

void USART1_Init(void);

uint8_t SPI2_ReadWriteByte(uint8_t Data);

void LCD_Write(uint8_t Cmd, uint32_t Size, uint8_t *Data);

void LCD_Init(void);
void LCD_SetStat(LCD_Stat Stat);
void LCD_Reset(void);
void LCD_PostInit(void);

void LCD_SetPos(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2);

void LCD_Clear(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t Color);
void LCD_DrawRect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t *Data);

void KEY_Init(void);
void KEY_SetRowStat(uint8_t Row, Key_Stat Stat);
Key_Stat Key_GetColStat(uint8_t Col);

void Board_Init(void);

#endif
