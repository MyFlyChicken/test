#ifndef _PHY_UART_H
#define _PHY_UART_H

#include "stm32f1xx_hal.h"
#include "Public.h"

#define RX_BUF_GROUP_NUM    2
#define RX_BUF_MAXNUM   20
#define RX_FRAME_HEADER_NUM 4
#define RX_BUF_DATA_LEN (RX_BUF_MAXNUM-RX_FRAME_HEADER_NUM)

#define TX_BUF_MAXNUM   40
#define TX_FRAME_HEADER_NUM 4
#define TX_BUF_DATA_LEN (TX_BUF_MAXNUM-TX_FRAME_HEADER_NUM)//待定

typedef struct _RX_BUFFER_{
    unsigned char ucFreeFlag;
    unsigned char ucLen;
    unsigned char ucRxBuffer[RX_BUF_MAXNUM];    
}RX_BUFFER;


typedef struct _TX_DATA_{
    unsigned char ucAddr;
    unsigned char ucCmd;
    unsigned char ucSubCmd;
    unsigned char ucDateLen;
    unsigned char ucData[TX_BUF_DATA_LEN];//包含CRC
}TX_DATA;
typedef union _TX_FRAME_{
    unsigned char ucBuffer[TX_BUF_MAXNUM];
    TX_DATA xTxData;
}TX_FRAME;


typedef struct _RX_DATA_{
    unsigned char ucAddr;
    unsigned char ucCmd;//最高位置1表示接收数据不对
    unsigned char ucSubCmd;
    unsigned char ucDateLen;
    unsigned char ucData[RX_BUF_DATA_LEN];//包含CRC
}RX_DATA;
typedef union _RX_FRAME_{
    unsigned char ucBuffer[RX_BUF_MAXNUM];
    RX_DATA xRxData;
}RX_FRAME;


extern  BOOL UART_RST_FLAG;
extern  TX_FRAME xTxFrame;
extern  RX_BUFFER *pxRxBuffer;


#define PHY_USART1_SET(b)  (UART_RST_FLAG = b)
#define PHY_USART1_ISRST() (UART_RST_FLAG == TRUE)


void Phy_UART1_Init(void);
void Phy_UART1_ReceiveIT(void);
void Phy_UART1_ErrDealIT(void);
void Phy_UART1_DisableRxIT(void);
BOOL Phy_UART1_EnableRxIT(void);
BOOL Phy_UART1_IS_ReceiveOver(void);
void Phy_UART1_Transmit(unsigned char *p, unsigned char ucLen);
void Phy_UART1_RxBufferRelease(RX_BUFFER *p);
void Phy_UART1_MainIn2MS(void);


#endif

