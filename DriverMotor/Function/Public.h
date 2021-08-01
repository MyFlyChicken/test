#ifndef _PUBLIC_H_
#define _PUBLIC_H_

typedef enum _BOOL_{
    FALSE = 0,
    TRUE
}BOOL;


#define MY_ABS(val) (val>0 ? val : 0 - val)
#define MY_MAX(a,b) (a>b ? a:b)
#define MY_MIN(a,b) (a<b ? a:b)


#define TIME_10MS_IN2MS  5
#define TIME_20MS_IN2MS  10
#define TIME_1S_IN2MS    500

unsigned short N_CRC16(unsigned char *updata, unsigned char len);





#endif
