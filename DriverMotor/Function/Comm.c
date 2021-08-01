#include "Function.h"
#include "Phy.h"


COMM_CMD eCommCmd;
COMM_PARSED_VAL xCommParsedVal;


static unsigned char ucSubCmd;
static COMM_DEAL_RES eCommDealRes;


static void Comm_UpdateTxBuffer(RX_FRAME *pxRxFrame, COMM_DEAL_RES eRes);
static void Comm_DealCmd0(RX_FRAME *pxRxFrame);
static void Comm_DealCmd1(RX_FRAME *pxRxFrame);
static void Comm_DealCmd2(RX_FRAME *pxRxFrame);
static void Comm_DealCmd3(RX_FRAME *pxRxFrame);
static void Comm_DealCmd4(RX_FRAME *pxRxFrame);



void Comm_VariableInit(void)
{
	ucSubCmd = 0;
    eCommCmd = CMD_EN_CYCLE;
}



void Comm_CmdDeal(unsigned char *p)
{   
    RX_FRAME *pxRxFrame;    
    pxRxFrame = (RX_FRAME *)p;
    if(ADDR != pxRxFrame->xRxData.ucAddr)
    {
        return;
    }    
    if(0 != N_CRC16(pxRxFrame->ucBuffer, (pxRxFrame->xRxData.ucDateLen + RX_FRAME_HEADER_NUM + 2)))//地址 命令 数据长度 CRC值
    {
        return;
    }
	ucSubCmd = pxRxFrame->xRxData.ucSubCmd;
    switch(pxRxFrame->xRxData.ucCmd)
    {
        case CMD_NONE:
            Comm_DealCmd0(pxRxFrame);
        break;
        case CMD_EN_CYCLE:
            Comm_DealCmd1(pxRxFrame);          
        break;
        case CMD_RD_ENCODER_VAL:
            Comm_DealCmd2(pxRxFrame);
        break;
        case CMD_WR_PWM_PULSE:
            Comm_DealCmd3(pxRxFrame);
        break;            
        case CMD_WR_PID_PARA:
            Comm_DealCmd4(pxRxFrame);
        break;   
        default:
            eCommDealRes = COMM_DEAL_CMD_ERR;
        break;
    }    
    Comm_UpdateTxBuffer(pxRxFrame, eCommDealRes);
}


void Comm_TransmitCmdMode(TX_FRAME *p)
{
    Phy_UART1_Transmit(p->ucBuffer, (p->xTxData.ucDateLen + TX_FRAME_HEADER_NUM + 2));
}


//ucRes 0成功 1长度不匹配 2没有该功能 3当前处于PID调参模式，不能设置PWM
static void Comm_UpdateTxBuffer(RX_FRAME *pxRxFrame, COMM_DEAL_RES eRes)
{
    unsigned short usTemp; 
    xTxFrame.xTxData.ucAddr = pxRxFrame->xRxData.ucAddr;
    xTxFrame.xTxData.ucCmd = pxRxFrame->xRxData.ucCmd;
    xTxFrame.xTxData.ucSubCmd = pxRxFrame->xRxData.ucSubCmd;
    xTxFrame.xTxData.ucDateLen = 1;
    xTxFrame.xTxData.ucData[0] = eRes;//结果在这里
    usTemp = N_CRC16(pxRxFrame->ucBuffer, xTxFrame.xTxData.ucDateLen + TX_FRAME_HEADER_NUM);//地址 命令 子命令 数据长度 数据
    xTxFrame.xTxData.ucData[1] = usTemp>>8;
    xTxFrame.xTxData.ucData[2] = usTemp&0xFF;
}

//命令为0的处理
static void Comm_DealCmd0(RX_FRAME *pxRxFrame)
{
    if(0 == pxRxFrame->xRxData.ucCmd)
    {
        if(0 == pxRxFrame->xRxData.ucSubCmd)
        {
            eCommCmd = CMD_NONE;  
            eCommDealRes = COMM_DEAL_OK;
        }
        else
        {            
            eCommDealRes = COMM_DEAL_SUBCMD_ERR;
        }
    }
    else
    {
        eCommDealRes = COMM_DEAL_CMD_ERR;
    }
}

//命令为1的处理
static void Comm_DealCmd1(RX_FRAME *pxRxFrame)
{
    if(1 == pxRxFrame->xRxData.ucCmd)
    {
        if(DISABLE_CYCLE == pxRxFrame->xRxData.ucSubCmd)
        {
            eCommCmd = CMD_NONE;  
            eCommDealRes = COMM_DEAL_OK;
        }
        else if(ENABLE_CYCLE == pxRxFrame->xRxData.ucSubCmd)
        {
            eCommCmd = CMD_EN_CYCLE;
            eCommDealRes = COMM_DEAL_OK;
        }
        else
        {
            eCommDealRes = COMM_DEAL_SUBCMD_ERR;
        }
    }
    else
    {
        eCommDealRes = COMM_DEAL_CMD_ERR;
    }
}

//命令为2的处理
static void Comm_DealCmd2(RX_FRAME *pxRxFrame)
{
    if(2 == pxRxFrame->xRxData.ucCmd)
    {
        eCommCmd = CMD_RD_ENCODER_VAL;
        if(0 == pxRxFrame->xRxData.ucSubCmd)
        {
            eCommDealRes = COMM_DEAL_OK;
        }
        else
        {
            eCommDealRes = COMM_DEAL_SUBCMD_ERR;
        }
    }
    else
    {
        eCommDealRes = COMM_DEAL_CMD_ERR;
    }
}

//命令为3的处理 PID调试时无法设置PWM占空比
static void Comm_DealCmd3(RX_FRAME *pxRxFrame)
{
    unsigned short sTemp;
    if(CMD_WR_PID_PARA == eCommCmd)//
    {
        eCommDealRes = COMM_DEAL_ISPID_MODE;
        return;
    }
    if(3 == pxRxFrame->xRxData.ucCmd)
    {
        eCommCmd = CMD_WR_PWM_PULSE;
        if(0 == pxRxFrame->xRxData.ucSubCmd)
        {
            if(2 != pxRxFrame->xRxData.ucDateLen)
            {                
                eCommDealRes = COMM_DEAL_DATALEN_ERR;
                return;
            }
            sTemp = pxRxFrame->xRxData.ucData[0];
            sTemp <<= 8;
            sTemp += pxRxFrame->xRxData.ucData[1];
            PWM1_SETPULSE(sTemp);
            eCommDealRes = COMM_DEAL_OK;
        }
        else if(1 == pxRxFrame->xRxData.ucSubCmd)
        {
            if(2 != pxRxFrame->xRxData.ucDateLen)
            {                
                eCommDealRes = COMM_DEAL_DATALEN_ERR;
                return;
            }
            sTemp = pxRxFrame->xRxData.ucData[0];
            sTemp <<= 8;
            sTemp += pxRxFrame->xRxData.ucData[1];
            PWM2_SETPULSE(sTemp);
            eCommDealRes = COMM_DEAL_OK;
        }
        else
        {            
            eCommDealRes = COMM_DEAL_SUBCMD_ERR;
        }
    }    
    else
    {
        eCommDealRes =  COMM_DEAL_CMD_ERR;
    }
}

//命令为4的处理
static void Comm_DealCmd4(RX_FRAME *pxRxFrame)
{
    unsigned char i,bit;
    signed short sTemp;
    if(4 == pxRxFrame->xRxData.ucCmd)
    {
        eCommCmd = CMD_WR_PID_PARA;
        if(0 == pxRxFrame->xRxData.ucSubCmd)//期望值
        {
            if((3 == pxRxFrame->xRxData.ucDateLen)||(5 == pxRxFrame->xRxData.ucDateLen))
            {
                for(i = 0; i<MAX_PID_PARA_NUM; i++)
                {
					bit = 0x01<<i; 
                    if(0 == (bit&pxRxFrame->xRxData.ucData[0]))
                        continue;
                    Pid_ParaInit(i);
                    ucPidIdex |= 0x01<<i;//先设置期望值pid调试才能够进行
                    sTemp = ((signed short)pxRxFrame->xRxData.ucData[1 + i*2]<<8) + pxRxFrame->xRxData.ucData[2 + i*2];
                    Pid_SetExpectedVal(&xPidPara[i], sTemp);  
                }
                eCommDealRes = COMM_DEAL_OK;
            }
            else
            {
                eCommDealRes = COMM_DEAL_DATALEN_ERR;
                return;
            }
        }
        else if(1 == pxRxFrame->xRxData.ucSubCmd)//设置kp ki kd的值
        {
            if((7 == pxRxFrame->xRxData.ucDateLen)||(13 == pxRxFrame->xRxData.ucDateLen))
            {
                for(i = 0; i<MAX_PID_PARA_NUM; i++)
                {
					bit = 0x01<<i; 
                    if(0 == (bit&pxRxFrame->xRxData.ucData[0]))
                        continue;
                    Pid_SetPara(&xPidPara[i], &pxRxFrame->xRxData.ucData[1 + i*6]);//没有进行限     位  
                }
                eCommDealRes = COMM_DEAL_OK;
            }
            else
            {
                eCommDealRes = COMM_DEAL_DATALEN_ERR;
                return;
            }
        }
        else if(2 == pxRxFrame->xRxData.ucSubCmd)//复位pid的值
        {
            if(1 == pxRxFrame->xRxData.ucDateLen)
            {              
                for(i = 0; i<MAX_PID_PARA_NUM; i++)
                {
					bit = 0x01<<i; 
                    if(0 == (bit&pxRxFrame->xRxData.ucData[0]))
                        continue;
                    Pid_ParaInit(i);
					ucPidIdex &= (~bit);
                }
                eCommDealRes = COMM_DEAL_OK;
            }
            else
            {
                eCommDealRes = COMM_DEAL_DATALEN_ERR;
                return;
            }
        }
        else if(3 == pxRxFrame->xRxData.ucSubCmd)//pid计算周期
        {
            if(2 == pxRxFrame->xRxData.ucDateLen)
            {
                ucCalPidParaCycle = ((unsigned short)pxRxFrame->xRxData.ucData[0]<<8) + pxRxFrame->xRxData.ucData[1];
                if(0 != ucCalPidParaCycle%2)
                {
                    ucCalPidParaCycle += 1; 
                }
                ucCalPidParaCycle /= 2;//计数周期为2ms计数一次，故此处除以2
                if((ucCalPidParaCycle > CAL_PIDPARA_MAX_TIME)||(ucCalPidParaCycle < CAL_PIDPARA_MIN_TIME))
                {
                    ucCalPidParaCycle = CAL_PIDPARA_MAX_TIME;
                }
                eCommDealRes = COMM_DEAL_OK;
            }
            else
            {
                eCommDealRes = COMM_DEAL_DATALEN_ERR;
                return;
            }
                
        }
        else if(4 == pxRxFrame->xRxData.ucSubCmd)//关闭pid调试           是否要停止电机？
        {
			if(1 == pxRxFrame->xRxData.ucDateLen)
			{
                eCommCmd = CMD_EN_CYCLE;
				for(i = 0; i<MAX_PID_PARA_NUM; i++)
				{
					bit = 0x01<<i;
					if(0 == (bit&pxRxFrame->xRxData.ucData[0]))
                        continue;
					Pid_ParaInit(i);
				}
				eCommDealRes = COMM_DEAL_OK;
			}
			else
			{
				eCommDealRes = COMM_DEAL_DATALEN_ERR;
                return;
			}
        }
        else
        {            
            eCommCmd = CMD_NONE;
            eCommDealRes = COMM_DEAL_SUBCMD_ERR;
        }
    }    
    else
    {
        eCommDealRes = COMM_DEAL_CMD_ERR;
    }
}

