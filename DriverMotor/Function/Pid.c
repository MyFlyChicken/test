#include "Function.h"
#include "Phy.h"


unsigned char ucPidIdex;//bit0 为1 pid1控制使能  bit1 pid2控制使能 
unsigned char ucCalPidParaCycle;//PID周期控制时间，4ms~20ms
PID_PARA xPidPara[MAX_PID_PARA_NUM];


//PID参数初始化
void Pid_ParaInit(unsigned char ucPidId)
{
    if(ucPidId >= MAX_PID_PARA_NUM)
    {
        return;
    }
    xPidPara[ucPidId].sKp = 0;
    xPidPara[ucPidId].sKi = 0;
    xPidPara[ucPidId].sKd = 0;

    //保证精度对以下参数进行放大100倍
    xPidPara[ucPidId].sExpectedVal = 0;
    xPidPara[ucPidId].lCurrErr = 0;
    xPidPara[ucPidId].sActualVal = 0;
    xPidPara[ucPidId].lLastErr = 0;
    xPidPara[ucPidId].lLastLastErr = 0;
    xPidPara[ucPidId].lHistoryErrSum = 0;

}

//设置P I D参数, 通讯调用
void Pid_SetPara(PID_PARA *pxPidPara, unsigned char *p)
{
    pxPidPara->sKp = (((signed short)*(p+0))<<8) + *(p+1);
    pxPidPara->sKi = (((signed short)*(p+2))<<8) + *(p+3);
    pxPidPara->sKd = (((signed short)*(p+4))<<8) + *(p+5);
}

void Pid_SetExpectedVal(PID_PARA *pxPidPara, signed short sVal)
{
    if(sVal > (signed short)PID_MAX_EXPECTED_VAL)
        sVal = PID_MAX_EXPECTED_VAL;
    pxPidPara->sExpectedVal = sVal;
}

//滤波 什么情况下需要滤波，采用哪种滤波方式

