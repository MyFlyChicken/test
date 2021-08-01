#ifndef _PHY_GPIO_H
#define _PHY_GPIO_H

#include "stm32f1xx_hal.h"
#include "Public.h"


//两路GPIO信号组合控制电机正反转
#define GPIO_MOTORA1 (GPIOC)
#define GPIO_MOTORA2 (GPIOC)
#define GPIO_PIN_MOTORA1 (GPIO_PIN_4 + 0)
#define GPIO_PIN_MOTORA2 (GPIO_PIN_5 + 0)

#define GPIO_MOTORB1 (GPIOC)
#define GPIO_MOTORB2 (GPIOC)
#define GPIO_PIN_MOTORB1 (GPIO_PIN_1 + 0)
#define GPIO_PIN_MOTORB2 (GPIO_PIN_2 + 0)




typedef enum _MOTOR_DIR_CTR_{
    MOTOR_STOP = 0,
    MOTOR_FORWARD,
    MOTOR_REVERSE,
    MOTOR_BRAKING
}MOTOR_DIR_CTR;



#define LED_CTRL(b) (b ? (GPIOB->BSRR = GPIO_PIN_5):(GPIOB->BSRR = (GPIO_PIN_5<<16)))


void Phy_GPIO_Init(void);
void Phy_GpioMotorCtr1(MOTOR_DIR_CTR eMotorDirCtr);
void Phy_GpioMotorCtr2(MOTOR_DIR_CTR eMotorDirCtr);


#endif

