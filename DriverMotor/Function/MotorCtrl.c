#include "Function.h"
#include "Phy.h"


const void (* const Function[2])(MOTOR_DIR_CTR) = {Phy_GpioMotorCtr1, Phy_GpioMotorCtr2};


//�������������             ucMode�������M1 0����M0
void MotorCtr_Main(PID_PARA *pxPidPara, unsigned char ucMode)
{
    long lTemp;
    if(ucMode>MAX_PID_PARA_NUM)
        return;
if(pxPidPara->sActualVal>=0)
{
	pxPidPara->lCurrErr = pxPidPara->sExpectedVal - pxPidPara->sActualVal;
}else{
	pxPidPara->lCurrErr = pxPidPara->sExpectedVal + pxPidPara->sActualVal;
}    
#if (PID_CONTROL_POSITION_MODE==PID_CONTROL_MODE)//λ��ʽ
    pxPidPara->lHistoryErrSum = pxPidPara->lLastLastErr + pxPidPara->lLastErr + pxPidPara->lCurrErr;
    lTemp = pxPidPara->sKp*pxPidPara->lCurrErr + pxPidPara->sKi*pxPidPara->lHistoryErrSum + pxPidPara->sKd*(pxPidPara->lCurrErr-pxPidPara->lLastErr);    
#elif (PID_CONTROL_DELTA_MODE==PID_CONTROL_MODE)//����ʽ
    lTemp = pxPidPara->sKp*pxPidPara->lCurrErr + pxPidPara->sKi*pxPidPara->lCurrErr + pxPidPara->sKd*(pxPidPara->lCurrErr + pxPidPara->lLastLastErr - 2*pxPidPara->lLastErr);
#endif
    pxPidPara->lLastLastErr = pxPidPara->lLastErr;
    pxPidPara->lLastErr = pxPidPara->lCurrErr;
    if(lTemp>0)//�������� ��ת
    {   
        Function[ucMode](MOTOR_FORWARD);
    }
    else if(lTemp<0)//���С���� ��ת
    {
        Function[ucMode](MOTOR_REVERSE);
    }
    /*else//Ϊ0ͣת
    {
        Function[ucMode](MOTOR_STOP);
    }*/    
    lTemp = MY_ABS(lTemp);
    PWM1_SETPULSE(lTemp);
}
