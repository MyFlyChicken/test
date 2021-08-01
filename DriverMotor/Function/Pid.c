#include "Function.h"
#include "Phy.h"


unsigned char ucPidIdex;//bit0 Ϊ1 pid1����ʹ��  bit1 pid2����ʹ�� 
unsigned char ucCalPidParaCycle;//PID���ڿ���ʱ�䣬4ms~20ms
PID_PARA xPidPara[MAX_PID_PARA_NUM];


//PID������ʼ��
void Pid_ParaInit(unsigned char ucPidId)
{
    if(ucPidId >= MAX_PID_PARA_NUM)
    {
        return;
    }
    xPidPara[ucPidId].sKp = 0;
    xPidPara[ucPidId].sKi = 0;
    xPidPara[ucPidId].sKd = 0;

    //��֤���ȶ����²������зŴ�100��
    xPidPara[ucPidId].sExpectedVal = 0;
    xPidPara[ucPidId].lCurrErr = 0;
    xPidPara[ucPidId].sActualVal = 0;
    xPidPara[ucPidId].lLastErr = 0;
    xPidPara[ucPidId].lLastLastErr = 0;
    xPidPara[ucPidId].lHistoryErrSum = 0;

}

//����P I D����, ͨѶ����
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

//�˲� ʲô�������Ҫ�˲������������˲���ʽ

