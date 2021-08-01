#ifndef _COMM_H_
#define _COMM_H_

#include "Phy_UART.h"


#define ADDR        1
#define DISABLE_CYCLE   0
#define ENABLE_CYCLE    1

typedef enum _COMM_DEAL_RES_{
    COMM_DEAL_OK = 0,
    COMM_DEAL_CMD_ERR = 1,
    COMM_DEAL_SUBCMD_ERR = 2,
    COMM_DEAL_ISPID_MODE = 3,
    COMM_DEAL_DATALEN_ERR = 4
}COMM_DEAL_RES;




typedef enum _COMM_CMD_{
    CMD_NONE = 0,
    CMD_EN_CYCLE,
    CMD_RD_ENCODER_VAL,//两个一起读
    CMD_WR_PWM_PULSE,
    CMD_WR_PID_PARA    
}COMM_CMD;

typedef struct _COMM_PARSED_VAL_{
    unsigned short PWM1_PULSE;
    unsigned short PWM2_PULSE;
}COMM_PARSED_VAL;

extern COMM_CMD eCommCmd;
extern COMM_PARSED_VAL xCommParsedVal;


#define COMM_CMD_RST() (eCommCmd = CMD_NONE);


void Comm_VariableInit(void);
void Comm_CmdDeal(unsigned char *p);
void Comm_TransmitCmdMode(TX_FRAME *p);

#endif
