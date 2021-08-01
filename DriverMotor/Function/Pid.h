#ifndef _PID_H_
#define _PID_H_

#define PID_CONTROL_POSITION_MODE   0//λ��ʽPID
#define PID_CONTROL_DELTA_MODE      1//����ʽPID
#define PID_CONTROL_MODE    (PID_CONTROL_DELTA_MODE)//0λ��PID 1

//���ֵ32767
#define PID_MAX_EXPECTED_VAL    0x7FFF

//PID   ��������Ϊ0.1
typedef struct _PID_PARA_{
    short sKp;
    short sKi;
    short sKd;
    
    short sExpectedVal;//����ֵ��ʱΪ�����������һȦ��Ҫ������Ϊ=���һȦ����*4*���ٱ�
    short sActualVal;//ʵ��ֵ
    long lCurrErr;//��������Խ�磬�����Ҫ��long�ʹ洢
    long lLastErr;
    long lLastLastErr;
    long lHistoryErrSum;    
}PID_PARA;

#define MAX_PID_PARA_NUM    2
#define PID_CTRL_PARA1      0
#define PID_CTRL_PARA2      1
extern PID_PARA xPidPara[MAX_PID_PARA_NUM];

#define CAL_PIDPARA_MIN_TIME 2
#define CAL_PIDPARA_MAX_TIME 20 
extern unsigned char ucCalPidParaCycle; ////��2ms���ڽ��м��� ����ֵ������2�������������õĲ�����ǿ���޸�
extern unsigned char ucPidIdex;



void Pid_ParaInit(unsigned char ucPidId);
void Pid_SetPara(PID_PARA *pxPidPara, unsigned char *p);
void Pid_SetExpectedVal(PID_PARA *pxPidPara, signed short sVal);




#endif



