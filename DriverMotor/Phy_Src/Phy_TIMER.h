#ifndef _PHY_TIMER_H
#define _PHY_TIMER_H

#include "stm32f1xx_hal.h"
#include "Public.h"

#define PWM_DEFAULT_PSC     71
#define PWM_DEFAULT_ARR     999
#define PWM_MIN_VAL         0
#define PWM_MAX_VAL         999

#define ENCODER_DEFAULT_PSC 0
#define ENCODER_DEFAULT_ARR 0xffff



#define PWM1_SETPULSE(usVal) (usVal>PWM_MAX_VAL ? (TIM1->CCR1 = PWM_MAX_VAL):(TIM1->CCR1 = usVal))
#define PWM2_SETPULSE(usVal) (usVal>PWM_MAX_VAL ? (TIM1->CCR4 = PWM_MAX_VAL):(TIM1->CCR4 = usVal))

//������������㿪ʼ��������ת������ȷ��ȡ��Ϣ(��������0��Ϊ65535)�����Դ�ֵΪ��㣬2ms��ȡһ�μ�����ֵ�����ڸ�ֵΪ��ת��С��Ϊ��ת
#define ENCODER_ZERO_VAL	 0x8000
#define ENCODER_NUM		     2
#define ENCODER1_VAL_INDEX   0
#define ENCODER2_VAL_INDEX   1
typedef struct _ENCODER_INFO_{
	long lPulse;
	short sCurrPulse;
}ENCODER_INFO;
extern ENCODER_INFO xEncoderInfo[ENCODER_NUM];

#define GET_ENCODER_PULSE(idex)	(xEncoderInfo[idex].lPulse + 0)
#define GET_ENCODER_CURR_PULSE(idex)	(xEncoderInfo[idex].sCurrPulse + 0)



void Phy_TimerInit(void);
void Phy_PWM1SetPulse(unsigned short usVal);
void Phy_PWM2SetPulse(unsigned short usVal);
void Phy_GetEncodeVal(unsigned char ucVal);
void Phy_TimerMainIn2MS(void);


#endif

