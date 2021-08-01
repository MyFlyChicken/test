#ifndef _MPTOR_CTRL_H_
#define _MPTOR_CTRL_H_

#define MOTOR_ENCODER_VAL       11
#define MOTOR_REDUCTION_RATIO   44//编码器精度为0.1，所以减速比扩大10倍


//编码器取反待定
void MotorCtr_Main(PID_PARA *pxPidPara, unsigned char ucMode);



#endif


