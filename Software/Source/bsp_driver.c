#include "stm32f10x.h"
#include "bsp_driver.h"
#include "ucos_ii.h"
#include "usb_init.h"
#include "hw_config.h"

const GPIO_InitDef LED_PIN =          {GPIOC, GPIO_Pin_13, GPIO_Speed_50MHz, GPIO_Mode_Out_PP};
const GPIO_InitDef USART1_TX_PIN =    {GPIOA, GPIO_Pin_9, GPIO_Speed_50MHz, GPIO_Mode_AF_PP};
const GPIO_InitDef USART1_RX_PIN =    {GPIOA, GPIO_Pin_10, GPIO_Speed_50MHz, GPIO_Mode_IN_FLOATING};
const GPIO_InitDef SPI2_SCK_PIN =     {GPIOB, GPIO_Pin_13, GPIO_Speed_50MHz, GPIO_Mode_AF_PP};
const GPIO_InitDef SPI2_MISO_PIN =    {GPIOB, GPIO_Pin_14, GPIO_Speed_50MHz, GPIO_Mode_IPU};
const GPIO_InitDef SPI2_MOSI_PIN =    {GPIOB, GPIO_Pin_15, GPIO_Speed_50MHz, GPIO_Mode_AF_PP};
const GPIO_InitDef LCD_LED_PIN =      {GPIOB, GPIO_Pin_9, GPIO_Speed_50MHz, GPIO_Mode_Out_PP};
const GPIO_InitDef LCD_RS_PIN =       {GPIOB, GPIO_Pin_10, GPIO_Speed_50MHz, GPIO_Mode_Out_PP};
const GPIO_InitDef LCD_CS_PIN =       {GPIOB, GPIO_Pin_11, GPIO_Speed_50MHz, GPIO_Mode_Out_PP};
const GPIO_InitDef LCD_RST_PIN =      {GPIOB, GPIO_Pin_12, GPIO_Speed_50MHz, GPIO_Mode_Out_PP};
const GPIO_InitDef KEY_R1_PIN =       {GPIOA, GPIO_Pin_3, GPIO_Speed_50MHz, GPIO_Mode_Out_PP};
const GPIO_InitDef KEY_R2_PIN =       {GPIOA, GPIO_Pin_2, GPIO_Speed_50MHz, GPIO_Mode_Out_PP};
const GPIO_InitDef KEY_R3_PIN =       {GPIOA, GPIO_Pin_1, GPIO_Speed_50MHz, GPIO_Mode_Out_PP};
const GPIO_InitDef KEY_R4_PIN =       {GPIOA, GPIO_Pin_0, GPIO_Speed_50MHz, GPIO_Mode_Out_PP};
const GPIO_InitDef KEY_C1_PIN =       {GPIOA, GPIO_Pin_4, GPIO_Speed_50MHz, GPIO_Mode_IPD};
const GPIO_InitDef KEY_C2_PIN =       {GPIOA, GPIO_Pin_5, GPIO_Speed_50MHz, GPIO_Mode_IPD};
const GPIO_InitDef KEY_C3_PIN =       {GPIOA, GPIO_Pin_6, GPIO_Speed_50MHz, GPIO_Mode_IPD};
const GPIO_InitDef KEY_C4_PIN =       {GPIOA, GPIO_Pin_7, GPIO_Speed_50MHz, GPIO_Mode_IPD};
const GPIO_InitDef FLASH_CS_PIN =     {GPIOB, GPIO_Pin_8, GPIO_Speed_50MHz, GPIO_Mode_Out_PP};
const GPIO_InitDef USB_DM_PIN =       {GPIOA, GPIO_Pin_11, GPIO_Speed_50MHz, GPIO_Mode_AF_PP};
const GPIO_InitDef USB_DP_PIN =       {GPIOA, GPIO_Pin_12, GPIO_Speed_50MHz, GPIO_Mode_AF_PP};

uint8_t LCD_PostInitCmd[] = {
    0x11, 1, 0x00,
    0xcf, 3, 0x00, 0xc1, 0x30,
    0xed, 4, 0x64, 0x03, 0x12, 0x81,
    0xe8, 3, 0x85, 0x11, 0x78,
    0xf6, 3, 0x01, 0x30, 0x00,
    0xcb, 5, 0x39, 0x2c, 0x00, 0x34, 0x05,
    0xf7, 1, 0x20,
    0xea, 2, 0x00, 0x00,
    0xc0, 1, 0x20,
    0xc1, 1, 0x11,
    0xc5, 2, 0x31, 0x3c,
    0xc7, 1, 0xa9,
    0x3a, 1, 0x55,
    0x36, 1, 0xe8, //0x48
    0xb1, 2, 0x00, 0x18,
    0xb4, 2, 0x00, 0x00,
    0xf2, 1, 0x00,
    0x26, 1, 0x01,
    0xe0, 15, 0x0f, 0x17, 0x14, 0x09, 0x0c, 0x06, 0x43, 0x75, 0x36, 0x08, 0x13, 0x05, 0x10, 0x0b, 0x08,
    0xe1, 15, 0x00, 0x1f, 0x23, 0x03, 0x0e, 0x04, 0x39, 0x25, 0x4d, 0x06, 0x0d, 0x0b, 0x33, 0x37, 0x0f,
    0x29, 0,
    0x2c, 0,
    0xff
};

uint8_t FlashSectorBuf[FLASH_SECTOR_SIZE];

void SysTick_Init()
{
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);
    SysTick_Config(rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC);
}

void LED_Init()
{
    GPIO_INIT(LED_PIN);
}

void LED_SetStat(LED_Stat Stat)
{
    GPIO_WriteBit(LED_PIN.GPIO_Port, LED_PIN.GPIO_Pin, Stat == LED_On ? Bit_RESET : Bit_SET);
}

void USART1_Init(void)
{
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

uint8_t SPI2_ReadWriteByte(uint8_t Data)
{
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI2, Data);

    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI2);
}

void LCD_Write(uint8_t Cmd, uint32_t Size, uint8_t *Data)
{
    GPIO_ResetBits(LCD_CS_PIN.GPIO_Port, LCD_CS_PIN.GPIO_Pin);

    GPIO_ResetBits(LCD_RS_PIN.GPIO_Port, LCD_RS_PIN.GPIO_Pin);
    SPI2_ReadWriteByte(Cmd);

    GPIO_SetBits(LCD_RS_PIN.GPIO_Port, LCD_RS_PIN.GPIO_Pin);
    for (uint32_t i = 0; i < Size; i++)
    {
        SPI2_ReadWriteByte(Data[i]);
    }

    GPIO_SetBits(LCD_CS_PIN.GPIO_Port, LCD_CS_PIN.GPIO_Pin);
}

void LCD_Init()
{
    GPIO_INIT(SPI2_SCK_PIN);
    GPIO_INIT(SPI2_MISO_PIN);
    GPIO_INIT(SPI2_MOSI_PIN);
    GPIO_INIT(LCD_LED_PIN);
    GPIO_INIT(LCD_RS_PIN);
    GPIO_INIT(LCD_CS_PIN);
    GPIO_INIT(LCD_RST_PIN);

    SPI_InitTypeDef  SPI2_Conf;

    SPI2_Conf.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI2_Conf.SPI_Mode = SPI_Mode_Master;
    SPI2_Conf.SPI_DataSize = SPI_DataSize_8b;
    SPI2_Conf.SPI_CPOL = SPI_CPOL_High;
    SPI2_Conf.SPI_CPHA = SPI_CPHA_2Edge;
    SPI2_Conf.SPI_NSS = SPI_NSS_Soft;
    SPI2_Conf.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI2_Conf.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI2_Conf.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI2_Conf);

    SPI_Cmd(SPI2, ENABLE);

    LCD_SetStat(LCD_On);
    LCD_Reset();
    LCD_PostInit();
    LCD_Clear(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, 0xffff);
}

void LCD_SetStat(LCD_Stat Stat)
{
    GPIO_WriteBit(LCD_LED_PIN.GPIO_Port, LCD_LED_PIN.GPIO_Pin, Stat == LCD_On ? Bit_SET : Bit_RESET);
}

void LCD_Reset()
{
    GPIO_ResetBits(LCD_RST_PIN.GPIO_Port, LCD_RST_PIN.GPIO_Pin);
    OSTimeDlyHMSM(0, 0, 0, 100);
    GPIO_SetBits(LCD_RST_PIN.GPIO_Port, LCD_RST_PIN.GPIO_Pin);
    OSTimeDlyHMSM(0, 0, 0, 50);
}

void LCD_PostInit()
{
    uint8_t *Cmd = LCD_PostInitCmd;
    uint8_t DataSize;

    while (*Cmd != 0xff) {
        DataSize = *(Cmd + 1);
        LCD_Write(*Cmd, DataSize, Cmd + 2);
        Cmd += DataSize + 2;
    }
}

void LCD_SetPos(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2)
{
    uint8_t PosData[4];

    PosData[0] = X1 >> 8;
    PosData[1] = X1;
    PosData[2] = X2 >> 8;
    PosData[3] = X2;
    LCD_Write(0x2a, 4, PosData);

    PosData[0] = Y1 >> 8;
    PosData[1] = Y1;
    PosData[2] = Y2 >> 8;
    PosData[3] = Y2;
    LCD_Write(0x2b, 4, PosData);
}

void LCD_Clear(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t Color)
{
    LCD_SetPos(X1, Y1, X2, Y2);

    GPIO_ResetBits(LCD_CS_PIN.GPIO_Port, LCD_CS_PIN.GPIO_Pin);

    GPIO_ResetBits(LCD_RS_PIN.GPIO_Port, LCD_RS_PIN.GPIO_Pin);
    SPI2_ReadWriteByte(0x2c);

    GPIO_SetBits(LCD_RS_PIN.GPIO_Port, LCD_RS_PIN.GPIO_Pin);
    for (uint32_t i = 0; i < (X2-X1) * (Y2-Y1); i++)
    {
        SPI2_ReadWriteByte(Color >> 8);
        SPI2_ReadWriteByte(Color);
    }

    GPIO_SetBits(LCD_CS_PIN.GPIO_Port, LCD_CS_PIN.GPIO_Pin);
}

void LCD_DrawRect(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint16_t *Data)
{
    LCD_SetPos(X1, Y1, X2, Y2);
    LCD_Write(0x2c, 2 * (X2-X1) * (Y2-Y1), (uint8_t*)Data);
}

void KEY_Init()
{
    GPIO_INIT(KEY_R1_PIN);
    GPIO_INIT(KEY_R2_PIN);
    GPIO_INIT(KEY_R3_PIN);
    GPIO_INIT(KEY_R4_PIN);
    GPIO_INIT(KEY_C1_PIN);
    GPIO_INIT(KEY_C2_PIN);
    GPIO_INIT(KEY_C3_PIN);
    GPIO_INIT(KEY_C4_PIN);
}

void KEY_SetRowStat(uint8_t Row, Key_Stat Stat)
{
    static const GPIO_InitDef* Rows[] = {&KEY_R1_PIN, &KEY_R2_PIN, &KEY_R3_PIN, &KEY_R4_PIN};
    GPIO_WriteBit(Rows[Row]->GPIO_Port, Rows[Row]->GPIO_Pin, Stat == Key_Set ? Bit_SET : Bit_RESET);
}

Key_Stat Key_GetColStat(uint8_t Col)
{
    static const GPIO_InitDef* Cols[] = {&KEY_C1_PIN, &KEY_C2_PIN, &KEY_C3_PIN, &KEY_C4_PIN};
    return GPIO_ReadInputDataBit(Cols[Col]->GPIO_Port, Cols[Col]->GPIO_Pin) == Bit_SET ? Key_Set : Key_Reset;
}

void FLASH_Init()
{
    GPIO_INIT(FLASH_CS_PIN);

    while (FLASH_ManufactDeviceID() != 0xef16);
}

uint16_t FLASH_ManufactDeviceID()
{
    uint16_t ManufactDeviceID = 0;

    GPIO_ResetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    SPI2_ReadWriteByte(0x90);
    SPI2_ReadWriteByte(0x00);
    SPI2_ReadWriteByte(0x00);
    SPI2_ReadWriteByte(0x00);

    ManufactDeviceID = SPI2_ReadWriteByte(0xff) << 8;
    ManufactDeviceID |= SPI2_ReadWriteByte(0xff);

    GPIO_SetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    return ManufactDeviceID;
}

void FLASH_Read(uint8_t *Buf, uint32_t Addr, uint16_t Size)
{
    GPIO_ResetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    SPI2_ReadWriteByte(0x03);
    SPI2_ReadWriteByte(Addr >> 16);
    SPI2_ReadWriteByte(Addr >> 8);
    SPI2_ReadWriteByte(Addr);

    for (uint16_t i = 0; i < Size; i++)
    {
        *Buf++ = SPI2_ReadWriteByte(0xff);
    }

    GPIO_SetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);
}

void FLASH_WaitForIdel()
{
    uint8_t FlashStatus;

    GPIO_ResetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    SPI2_ReadWriteByte(0x05);

    do
    {
        FlashStatus = SPI2_ReadWriteByte(0xff);
    }
    while (FlashStatus & 0x01);

    GPIO_SetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);
}

void FLASH_SetWriteStat(Flash_WriteStat Stat)
{
    GPIO_ResetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    if (Stat == Flash_WriteEnable)
    {
        SPI2_ReadWriteByte(0x06);
    }
    else
    {
        SPI2_ReadWriteByte(0x04);
    }

    GPIO_SetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    FLASH_WaitForIdel();
}

void FLASH_PageWrite(const uint8_t *Buf, uint32_t Addr)
{
    FLASH_SetWriteStat(Flash_WriteEnable);

    GPIO_ResetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    SPI2_ReadWriteByte(0x02);
    SPI2_ReadWriteByte(Addr >> 16);
    SPI2_ReadWriteByte(Addr >> 8);
    SPI2_ReadWriteByte(Addr);

    for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++)
    {
        SPI2_ReadWriteByte(*Buf++);
    }

    GPIO_SetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    FLASH_WaitForIdel();
}

void FLASH_SectorWrite(const uint8_t *Buf, uint32_t Addr)
{
    for (uint8_t i = 0; i < FLASH_PAGE_PER_SECTOR; i++)
    {
        FLASH_PageWrite(Buf, Addr);
        Buf  += FLASH_PAGE_SIZE;
        Addr += FLASH_PAGE_SIZE;
    }
}

void FLASH_SectorErase(uint32_t Addr)
{
    FLASH_SetWriteStat(Flash_WriteEnable);

    GPIO_ResetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    SPI2_ReadWriteByte(0x20);
    SPI2_ReadWriteByte(Addr >> 16);
    SPI2_ReadWriteByte(Addr >> 8);
    SPI2_ReadWriteByte(Addr);

    GPIO_SetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    FLASH_WaitForIdel();
}

void FLASH_Write(const uint8_t *Buf, uint32_t Addr, uint16_t Size)
{
    GPIO_ResetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    uint32_t SectorAddr = Addr / FLASH_SECTOR_SIZE * FLASH_SECTOR_SIZE;
    uint16_t SectorOffset = Addr % FLASH_SECTOR_SIZE;
    uint16_t WriteSize = SectorOffset + Size > FLASH_SECTOR_SIZE ? FLASH_SECTOR_SIZE - SectorOffset : Size;

    if (Size < WriteSize)
    {
        WriteSize = Size;
    }

    uint16_t i;
    uint8_t *p;

    if (SectorOffset != 0)
    {
        FLASH_Read(FlashSectorBuf, SectorAddr, FLASH_SECTOR_SIZE);

        for (i = 0, p = FlashSectorBuf + SectorOffset; i < WriteSize; i++)
        {
            *p++ = *Buf++;
        }

        FLASH_SectorErase(SectorAddr);
        FLASH_SectorWrite(FlashSectorBuf, SectorAddr);

        Buf  += WriteSize;
        Addr += WriteSize;
        Size -= WriteSize;
    }

    while (Size >= FLASH_SECTOR_SIZE)
    {
        FLASH_SectorErase(Addr);
        FLASH_SectorWrite(Buf, Addr);

        Buf  += FLASH_SECTOR_SIZE;
        Addr += FLASH_SECTOR_SIZE;
        Size -= FLASH_SECTOR_SIZE;
    }

    if (Size > 0)
    {
        FLASH_Read(FlashSectorBuf, Addr, FLASH_SECTOR_SIZE);

        for (i = 0, p = FlashSectorBuf; i < Size; i++)
        {
            *p++ = *Buf++;
        }

        FLASH_SectorErase(Addr);
        FLASH_SectorWrite(FlashSectorBuf, Addr);
    }

    GPIO_SetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);
}

uint8_t FLASH_Status()
{
    GPIO_ResetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    uint8_t FlashStatus;

    SPI2_ReadWriteByte(0x05);

    FlashStatus = SPI2_ReadWriteByte(0xff);

    GPIO_SetBits(FLASH_CS_PIN.GPIO_Port, FLASH_CS_PIN.GPIO_Pin);

    return FlashStatus;
}

void USB_Init2()
{
    GPIO_INIT(USB_DM_PIN);
    GPIO_INIT(USB_DP_PIN);
    
    MAL_Config();

    NVIC_InitTypeDef USB_IRQ_Conf;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    USB_IRQ_Conf.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    USB_IRQ_Conf.NVIC_IRQChannelPreemptionPriority = 2;
    USB_IRQ_Conf.NVIC_IRQChannelSubPriority = 0;
    USB_IRQ_Conf.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&USB_IRQ_Conf);

    USB_IRQ_Conf.NVIC_IRQChannel = USBWakeUp_IRQn;
    USB_IRQ_Conf.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_Init(&USB_IRQ_Conf);
    
    USB_Init();
}

void Board_Init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

    SysTick_Init();
    LED_Init();
    USART1_Init();
    LCD_Init();
    KEY_Init();
    FLASH_Init();
    USB_Init2();
}
