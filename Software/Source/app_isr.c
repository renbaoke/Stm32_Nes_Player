#include "stm32f10x.h"
#include "bsp_driver.h"
#include "ucos_ii.h"

#define BS      0x08
#define CR      0x0d
#define DEL     0x7f

extern OS_EVENT *ShellMbox;

uint8_t  RecvBuf0[USART1_BUF_SIZE];
uint8_t  RecvBuf1[USART1_BUF_SIZE];

uint8_t* RecvBuf = RecvBuf0;
uint16_t RecvCnt = 0;

void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        uint16_t Data = USART_ReceiveData(USART1);

        if (Data == CR) {
            RecvBuf[RecvCnt++] = '\0';
            RecvCnt = 0;
            OSMboxPost(ShellMbox, RecvBuf);
            if (RecvBuf == RecvBuf0)
            {
                RecvBuf = RecvBuf1;
            }
            else
            {
                RecvBuf = RecvBuf0;
            }
        } else if (Data == DEL) {
            if (RecvCnt > 0) {
                RecvCnt--;
                USART_SendData(USART1, Data);
            }
        } else if (RecvCnt < USART1_BUF_SIZE - 1) {
            RecvBuf[RecvCnt++] = Data;
            USART_SendData(USART1, Data);
        }
    }
}
