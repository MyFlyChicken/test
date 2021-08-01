#include "Phy.h"



UART_HandleTypeDef huart1;
RX_BUFFER xRxBuffer[RX_BUF_GROUP_NUM];
RX_BUFFER *pxRxBuffer;
TX_FRAME xTxFrame;


BOOL UART_RST_FLAG;

static BOOL Phy_UART1_GetFreeRxBuffer(void);

static void Phy_UART1_RxBufferInit(void);

void Phy_UART1_Init(void)
{
    PHY_USART1_SET(FALSE);
    Phy_UART1_RxBufferInit();
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_2;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
    Phy_UART1_EnableRxIT();    
}

//发送
void Phy_UART1_Transmit(unsigned char *p, unsigned char ucLen)
{
    unsigned char i;
    
    if(ucLen>TX_BUF_MAXNUM)
    {
        ucLen = TX_BUF_MAXNUM;
    }
    i = 0;
    while(i<ucLen)
    {
        USART1->DR = (*p++ & 0xFF);
        while((USART1->SR&USART_SR_TXE) != USART_SR_TXE);
        i++;
    }
}

//接收中断处理
void Phy_UART1_ReceiveIT(void)
{
    if((NULL == pxRxBuffer)||(pxRxBuffer->ucLen>=RX_BUF_MAXNUM))
    {
        //关闭串口中断，接收完毕
        Phy_UART1_DisableRxIT();
        return;
    }    
    pxRxBuffer->ucRxBuffer[pxRxBuffer->ucLen++] = (USART1->DR & 0x00FF);    	
}

//错误中断处理
void Phy_UART1_ErrDealIT(void)
{
    Phy_UART1_DisableRxIT();    
    //变量值位，while内重启开关串口
    PHY_USART1_SET(TRUE);
}

//判断缓冲区数据是否接收完毕
BOOL Phy_UART1_IS_ReceiveOver(void)
{
    static unsigned char ucLen;
    if(NULL == pxRxBuffer)
        return FALSE;
    if((0 == pxRxBuffer->ucLen)||(pxRxBuffer->ucLen != ucLen))
    {
        ucLen = pxRxBuffer->ucLen;
        return FALSE;
    }
    return TRUE;
}


//开启中断
BOOL Phy_UART1_EnableRxIT(void)
{
    if(!Phy_UART1_GetFreeRxBuffer())
    {        
        return FALSE;
    }
    SET_BIT(USART1->CR1, USART_CR1_PEIE);
    SET_BIT(USART1->CR3, USART_CR3_EIE);
    SET_BIT(USART1->CR1, USART_CR1_RXNEIE);   
		return TRUE;		
}

//关闭中断
void Phy_UART1_DisableRxIT(void)
{
    CLEAR_BIT(USART1->CR1, USART_CR1_RXNEIE);//清楚接收非空中断使能
    CLEAR_BIT(USART1->CR3, USART_CR3_EIE);//清除错误使能    
    CLEAR_BIT(USART1->CR3, USART_CR3_EIE);
}

//释放当前串口缓冲
void Phy_UART1_RxBufferRelease(RX_BUFFER *p)
{
    if(NULL != p)
    {
        p->ucLen = 0;
        p->ucFreeFlag = 0;
        p = NULL;
    }  
}

//2MS执行一次
void Phy_UART1_MainIn2MS(void)
{
    static unsigned char ucReceiveOver;
    if(PHY_USART1_ISRST())//检测串口是否需要初始化
    {
        Phy_UART1_Init();
        Comm_VariableInit();
    }
    
    if(Phy_UART1_IS_ReceiveOver())//10ms内收不到数据表示一帧完毕
    {
        ucReceiveOver++;
        if(5 == ucReceiveOver)//因为2ms周期执行，所以此处为5
        {
            Comm_CmdDeal((unsigned char *)&pxRxBuffer->ucRxBuffer);
            Phy_UART1_RxBufferRelease(pxRxBuffer);
        }
    }
    else
    {
        ucReceiveOver = 0;
    }
}

//获取空闲的串口接收缓冲
static BOOL Phy_UART1_GetFreeRxBuffer(void)
{
    unsigned char i;
    for(i = 0; i<RX_BUF_GROUP_NUM; i++)
    {
        if(0 == xRxBuffer[i].ucFreeFlag)
        {
            xRxBuffer[i].ucLen = 0;
            pxRxBuffer = &xRxBuffer[i];
            return TRUE;
        }    
    }    
    return FALSE;
}

//串口接收缓冲初始化
static void Phy_UART1_RxBufferInit(void)
{
    unsigned char i;
    for(i = 0; i<RX_BUF_GROUP_NUM; i++)
    {
        xRxBuffer[i].ucFreeFlag = 0;
    }    
}



