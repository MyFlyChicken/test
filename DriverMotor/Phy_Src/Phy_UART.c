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

//����
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

//�����жϴ���
void Phy_UART1_ReceiveIT(void)
{
    if((NULL == pxRxBuffer)||(pxRxBuffer->ucLen>=RX_BUF_MAXNUM))
    {
        //�رմ����жϣ��������
        Phy_UART1_DisableRxIT();
        return;
    }    
    pxRxBuffer->ucRxBuffer[pxRxBuffer->ucLen++] = (USART1->DR & 0x00FF);    	
}

//�����жϴ���
void Phy_UART1_ErrDealIT(void)
{
    Phy_UART1_DisableRxIT();    
    //����ֵλ��while���������ش���
    PHY_USART1_SET(TRUE);
}

//�жϻ����������Ƿ�������
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


//�����ж�
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

//�ر��ж�
void Phy_UART1_DisableRxIT(void)
{
    CLEAR_BIT(USART1->CR1, USART_CR1_RXNEIE);//������շǿ��ж�ʹ��
    CLEAR_BIT(USART1->CR3, USART_CR3_EIE);//�������ʹ��    
    CLEAR_BIT(USART1->CR3, USART_CR3_EIE);
}

//�ͷŵ�ǰ���ڻ���
void Phy_UART1_RxBufferRelease(RX_BUFFER *p)
{
    if(NULL != p)
    {
        p->ucLen = 0;
        p->ucFreeFlag = 0;
        p = NULL;
    }  
}

//2MSִ��һ��
void Phy_UART1_MainIn2MS(void)
{
    static unsigned char ucReceiveOver;
    if(PHY_USART1_ISRST())//��⴮���Ƿ���Ҫ��ʼ��
    {
        Phy_UART1_Init();
        Comm_VariableInit();
    }
    
    if(Phy_UART1_IS_ReceiveOver())//10ms���ղ������ݱ�ʾһ֡���
    {
        ucReceiveOver++;
        if(5 == ucReceiveOver)//��Ϊ2ms����ִ�У����Դ˴�Ϊ5
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

//��ȡ���еĴ��ڽ��ջ���
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

//���ڽ��ջ����ʼ��
static void Phy_UART1_RxBufferInit(void)
{
    unsigned char i;
    for(i = 0; i<RX_BUF_GROUP_NUM; i++)
    {
        xRxBuffer[i].ucFreeFlag = 0;
    }    
}



