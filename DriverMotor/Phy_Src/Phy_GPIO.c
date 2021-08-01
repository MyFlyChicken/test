#include "Phy.h"




void Phy_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  
  HAL_GPIO_WritePin(GPIO_MOTORA1, GPIO_PIN_MOTORA1, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO_PIN_MOTORA1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIO_MOTORA1, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIO_MOTORA2, GPIO_PIN_MOTORA2, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO_PIN_MOTORA2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIO_MOTORA2, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIO_MOTORB1, GPIO_PIN_MOTORB1, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO_PIN_MOTORB1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIO_MOTORB1, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIO_MOTORB2, GPIO_PIN_MOTORB2, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO_PIN_MOTORB2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIO_MOTORB2, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

void Phy_GpioMotorCtr1(MOTOR_DIR_CTR eMotorDirCtr)
{
    if(MOTOR_STOP == eMotorDirCtr)
    {
        GPIO_MOTORA1->BSRR = (unsigned long)GPIO_PIN_MOTORA1<<16;
        GPIO_MOTORA2->BSRR = (unsigned long)GPIO_PIN_MOTORA2<<16;
    }else if(MOTOR_FORWARD == eMotorDirCtr){
        GPIO_MOTORA1->BSRR = (unsigned long)GPIO_PIN_MOTORA1<<16;
        GPIO_MOTORA2->BSRR = GPIO_PIN_MOTORA2;
    }else if(MOTOR_REVERSE == eMotorDirCtr){
        GPIO_MOTORA1->BSRR = GPIO_PIN_MOTORA1;
        GPIO_MOTORA2->BSRR = (unsigned long)GPIO_PIN_MOTORA2<<16;
    }else if(MOTOR_BRAKING == eMotorDirCtr){
        GPIO_MOTORA1->BSRR = GPIO_PIN_MOTORA1;
        GPIO_MOTORA2->BSRR = GPIO_PIN_MOTORA2;
    }
    else
    {
        return;
    }
}

void Phy_GpioMotorCtr2(MOTOR_DIR_CTR eMotorDirCtr)
{
    if(MOTOR_STOP == eMotorDirCtr)
    {
        GPIO_MOTORB1->BSRR = (unsigned long)GPIO_PIN_MOTORB1<<16;
        GPIO_MOTORB2->BSRR = (unsigned long)GPIO_PIN_MOTORB2<<16;
    }else if(MOTOR_FORWARD == eMotorDirCtr){
        GPIO_MOTORB1->BSRR = (unsigned long)GPIO_PIN_MOTORB1<<16;
        GPIO_MOTORB2->BSRR = GPIO_PIN_MOTORB2;
    }else if(MOTOR_REVERSE == eMotorDirCtr){
        GPIO_MOTORB1->BSRR = GPIO_PIN_MOTORB1;
        GPIO_MOTORB2->BSRR = (unsigned long)GPIO_PIN_MOTORB2<<16;
    }else if(MOTOR_BRAKING == eMotorDirCtr){
        GPIO_MOTORB1->BSRR = GPIO_PIN_MOTORB1;
        GPIO_MOTORB2->BSRR = GPIO_PIN_MOTORB2;
    }
    else
    {
        return;
    }
}



