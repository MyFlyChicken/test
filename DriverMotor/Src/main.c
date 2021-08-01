/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>

static unsigned char ucSteup;//while处理步骤
static unsigned long ulCurrSysCnt;//当前系统时间
static unsigned long ulLastSysCnt;//上一次系统时间
static unsigned short usTimer1;
static unsigned short usTimer2;

static BOOL bLedCtr;

char SysRunning[20] = {'\0'};

void SystemClock_Config(void);
BOOL Sys_2msChk();


int main(void)
{
    unsigned char i;
    unsigned short usTemp;
    unsigned char ucCnt;
    HAL_Init();
    SystemClock_Config();
    Phy_TimerInit();
    Phy_UART1_Init();
    
    Phy_GPIO_Init();
    Comm_VariableInit();
    ucCnt = 0;
    ulCurrSysCnt = HAL_GetTick();
    ulLastSysCnt = HAL_GetTick();
    ucSteup = 0;
    usTimer1 = 0;
    usTimer2 = 0;
    ucCalPidParaCycle = 0;
    bLedCtr = FALSE;
    while (1)
    {
        if(TRUE == Sys_2msChk())//2ms到了
        {
            Phy_UART1_MainIn2MS();
            Phy_TimerMainIn2MS();
            usTimer1++;
            usTimer2++;
            xPidPara[0].sActualVal += GET_ENCODER_CURR_PULSE(0);
            xPidPara[1].sActualVal += GET_ENCODER_CURR_PULSE(1);
            if(ucCalPidParaCycle&&(CMD_WR_PID_PARA == eCommCmd))
            {
                if(0==(usTimer2%ucCalPidParaCycle))//进行PID参数整定并输出至PWM
                {
                    usTemp = 0x01;
                    for(i = 0; i<MAX_PID_PARA_NUM; i++)
                    {   
                        //进行PWM修正
                        if(ucPidIdex&usTemp)
                        {
                            //PID控制
                            MotorCtr_Main(&xPidPara[i], i);                    
                        }
                        usTemp <<= 1;
                    }
                    //目前仅支持单通道
                    xTxFrame.xTxData.ucAddr = ADDR;
                    xTxFrame.xTxData.ucCmd = CMD_WR_PID_PARA;
                    xTxFrame.xTxData.ucSubCmd = 3;
                    xTxFrame.xTxData.ucDateLen = 4;
                    xTxFrame.xTxData.ucData[0] = xPidPara[0].sExpectedVal>>8;
                    xTxFrame.xTxData.ucData[1] = xPidPara[0].sExpectedVal&0xFF;
                    xTxFrame.xTxData.ucData[2] = xPidPara[0].sActualVal>>8;
                    xTxFrame.xTxData.ucData[3] = xPidPara[0].sActualVal&0xFF;
                    usTemp = N_CRC16(xTxFrame.ucBuffer, (xTxFrame.xTxData.ucDateLen + TX_FRAME_HEADER_NUM));
                    xTxFrame.xTxData.ucData[4] = usTemp>>8;
                    xTxFrame.xTxData.ucData[5] = usTemp&0xFF;
                    Phy_UART1_Transmit(xTxFrame.ucBuffer, xTxFrame.xTxData.ucDateLen + TX_FRAME_HEADER_NUM + 2);
                }
            }       
        }                 
        if(TIME_1S_IN2MS == usTimer1)//1s执行函数
        {
            if(CMD_EN_CYCLE == eCommCmd)
            {
                xTxFrame.xTxData.ucAddr = ADDR;
                xTxFrame.xTxData.ucCmd = CMD_EN_CYCLE;
                xTxFrame.xTxData.ucSubCmd = 1;
                sprintf((char *)xTxFrame.xTxData.ucData, "%ld", ulCurrSysCnt);
                xTxFrame.xTxData.ucDateLen = strlen((char *)xTxFrame.xTxData.ucData);
                //输出当前系统滴答定时器的值
                usTemp = N_CRC16(xTxFrame.ucBuffer, (xTxFrame.xTxData.ucDateLen + TX_FRAME_HEADER_NUM));
                xTxFrame.xTxData.ucData[xTxFrame.xTxData.ucDateLen] = usTemp>>8;
                xTxFrame.xTxData.ucData[xTxFrame.xTxData.ucDateLen+1] = usTemp&0xFF;
                Phy_UART1_Transmit(xTxFrame.ucBuffer, xTxFrame.xTxData.ucDateLen + TX_FRAME_HEADER_NUM + 2);
            }
            else if(CMD_RD_ENCODER_VAL == eCommCmd)
            {
                //发送编码器值
                xTxFrame.xTxData.ucAddr = ADDR;
                xTxFrame.xTxData.ucCmd = CMD_RD_ENCODER_VAL;
                xTxFrame.xTxData.ucSubCmd = 0;
                xTxFrame.xTxData.ucDateLen = 8;
                xTxFrame.xTxData.ucData[0] = GET_ENCODER_PULSE(ENCODER1_VAL_INDEX)>>24;
                xTxFrame.xTxData.ucData[1] = (GET_ENCODER_PULSE(ENCODER1_VAL_INDEX)>>16)&0xFF;
                xTxFrame.xTxData.ucData[2] = (GET_ENCODER_PULSE(ENCODER1_VAL_INDEX)>>8)&0xFF;
                xTxFrame.xTxData.ucData[3] = GET_ENCODER_PULSE(ENCODER1_VAL_INDEX)&0xFF;
                xTxFrame.xTxData.ucData[4] = GET_ENCODER_PULSE(ENCODER2_VAL_INDEX)>>24;
                xTxFrame.xTxData.ucData[5] = (GET_ENCODER_PULSE(ENCODER2_VAL_INDEX)>>16)&0xFF;
                xTxFrame.xTxData.ucData[6] = (GET_ENCODER_PULSE(ENCODER2_VAL_INDEX)>>8)&0xFF;
                xTxFrame.xTxData.ucData[7] = GET_ENCODER_PULSE(ENCODER2_VAL_INDEX)&0xFF;
                usTemp = N_CRC16(xTxFrame.ucBuffer, (xTxFrame.xTxData.ucDateLen + TX_FRAME_HEADER_NUM));
                xTxFrame.xTxData.ucData[8] = usTemp>>8;
                xTxFrame.xTxData.ucData[9] = usTemp&0xFF;
                Phy_UART1_Transmit(xTxFrame.ucBuffer, xTxFrame.xTxData.ucDateLen + TX_FRAME_HEADER_NUM + 2);
            }
            else//发送当前模式
            {
                Comm_TransmitCmdMode(&xTxFrame);
            }
            usTimer1 = 0;
            LED_CTRL(bLedCtr);
            bLedCtr = !bLedCtr;
        }
    }
}
BOOL Sys_2msChk()
{
    ulCurrSysCnt = HAL_GetTick();
    if((ulCurrSysCnt-ulLastSysCnt)>=2)
    {
        ulLastSysCnt = ulCurrSysCnt;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
