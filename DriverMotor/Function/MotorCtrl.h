#ifndef _MPTOR_CTRL_H_
#define _MPTOR_CTRL_H_

#define MOTOR_ENCODER_VAL       11
#define MOTOR_REDUCTION_RATIO   44//����������Ϊ0.1�����Լ��ٱ�����10��


//������ȡ������
void MotorCtr_Main(PID_PARA *pxPidPara, unsigned char ucMode);



#endif


