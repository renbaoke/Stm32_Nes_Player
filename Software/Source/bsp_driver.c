#include "stm32f10x.h"
#include "bsp_driver.h"
#include "ucos_ii.h"

GPIO_InitDef LED_PIN =          {GPIOC, GPIO_Pin_13, GPIO_Speed_50MHz, GPIO_Mode_Out_PP};
GPIO_InitDef USART1_TX_PIN =    {GPIOA, GPIO_Pin_9, GPIO_Speed_50MHz, GPIO_Mode_AF_PP};
GPIO_InitDef USART1_RX_PIN =    {GPIOA, GPIO_Pin_10, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING};

void SysTick_Init()
{
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);
    SysTick_Config(rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC);
}

void LED_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_INIT(LED_PIN);
}

void LED_SetStat(LED_Stat Stat)
{
    GPIO_WriteBit(LED_PIN.GPIO_Port, LED_PIN.GPIO_Pin, Stat == LED_On ? Bit_RESET : Bit_SET);
}

void USART1_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_INIT(USART1_TX_PIN);
    GPIO_INIT(USART1_RX_PIN);

    USART_InitTypeDef USART1_Conf;
    USART1_Conf.USART_BaudRate = 115200;
    USART1_Conf.USART_WordLength = USART_WordLength_8b;
    USART1_Conf.USART_StopBits = USART_StopBits_1;
    USART1_Conf.USART_Parity = USART_Parity_No;
    USART1_Conf.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART1_Conf.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART1_Conf);

    NVIC_InitTypeDef USART1_IRQ_Conf;
    USART1_IRQ_Conf.NVIC_IRQChannel = USART1_IRQn;
    USART1_IRQ_Conf.NVIC_IRQChannelPreemptionPriority=3;
    USART1_IRQ_Conf.NVIC_IRQChannelSubPriority =3;
    USART1_IRQ_Conf.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&USART1_IRQ_Conf);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}

void Board_Init()
{
    SysTick_Init();
    LED_Init();
    USART1_Init();
}

