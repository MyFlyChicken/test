#ifndef _PID_H_
#define _PID_H_

#define PID_CONTROL_POSITION_MODE   0//位置式PID
#define PID_CONTROL_DELTA_MODE      1//增量式PID
#define PID_CONTROL_MODE    (PID_CONTROL_DELTA_MODE)//0位置PID 1

//最大值32767
#define PID_MAX_EXPECTED_VAL    0x7FFF

//PID   参数精度为0.1
typedef struct _PID_PARA_{
    short sKp;
    short sKi;
    short sKd;
    
    short sExpectedVal;//期望值暂时为脉冲数，输出一圈需要脉冲数为=电机一圈脉冲*4*减速比
    short sActualVal;//实际值
    long lCurrErr;//避免数据越界，误差需要以long型存储
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
extern unsigned char ucCalPidParaCycle; ////以2ms周期进行计数 设置值必须是2的整数倍，设置的不是则强制修复
extern unsigned char ucPidIdex;



void Pid_ParaInit(unsigned char ucPidId);
void Pid_SetPara(PID_PARA *pxPidPara, unsigned char *p);
void Pid_SetExpectedVal(PID_PARA *pxPidPara, signed short sVal);




#endif



