/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "hrtim.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "function.h"
#include "Key.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define REF_3V3 3.2993 // VREF参考电压
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// 毫秒计时变量
volatile uint16_t ms_cnt_1 = 0; // 计时变量1
volatile uint16_t ms_cnt_2 = 0; // 计时变量2
volatile uint16_t ms_cnt_3 = 0; // 计时变量3
volatile uint16_t ms_cnt_4 = 0; // 计时变量4

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_I2C3_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  MX_TIM8_Init();
  MX_ADC5_Init();
  MX_HRTIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3); // 启动定时器8和通道3的PWM输出
  FAN_PWM_set(100);                         // 设置风扇转速为100%
  OLED_Init();                              // OLED初始化
  HAL_TIM_Base_Start_IT(&htim2);            // 启动定时器2和定时器中断，1kHz
  Key_Init();                               // 按键状态机初始化

  HAL_Delay(100);                                                  // 延时100ms，等待供电稳定
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);           // 校准ADC1
  HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);           // 校准ADC2
  HAL_ADCEx_Calibration_Start(&hadc5, ADC_SINGLE_ENDED);           // 校准ADC5
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC1_RESULT, 4);           // 启动ADC1采样和DMA数据传送,采样输入输出电压电流
  HAL_ADC_Start(&hadc2);                                           // 启动ADC2采样，采样NTC温度
  HAL_ADC_Start(&hadc5);                                           // 启动ADC5采样，采样单片机CPU温度
  HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1|HRTIM_OUTPUT_TD2); //开启PWM输出和PWM计时器
	HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1|HRTIM_OUTPUT_TF2); //开启PWM输出和PWM计时器
	HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);//开启PWM输出和PWM计时器
	HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_F);//开启PWM输出和PWM计时器
	__HAL_HRTIM_TIMER_ENABLE_IT(&hhrtim1,HRTIM_TIMERINDEX_TIMER_D,HRTIM_TIM_IT_REP); // 开启HRTIM定时器D的中断

  HAL_GPIO_WritePin(GPIOC, LED_G_Pin | LED_R_Pin, GPIO_PIN_RESET); // 关闭LED_G和LED_R
  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET); // 关闭蜂鸣器

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  OLED_ShowString(0, 0, "VIN:", OLED_8X16); // 显示字符串
  OLED_ShowString(0, 16, "Iin:", OLED_8X16);
  OLED_ShowString(0, 32, "VOUT:", OLED_8X16);
  OLED_ShowString(0, 48, "IOUT:", OLED_8X16);

  OLED_Update();   // 更新OLED显示内容
  FAN_PWM_set(35); // 设置风扇转速为100%
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    Encoder();     // 编码器信号处理
    Key_Process(); // 按键按下的处理

    if (ms_cnt_3 >= 10) // 判断是否计时到10ms
    {
      ms_cnt_3 = 0;             // 计时清零
      BUZZER_Short();           // 蜂鸣器短促鸣叫
      KEY_Scan(1, KEY1);        // 按键1扫描
      KEY_Scan(2, KEY2);        // 按键2扫描
      KEY_Scan(3, Encoder_KEY); // 编码器按键扫描
    }

    if (ms_cnt_4 >= 50) // 判断是否计时到50ms
    {
      ms_cnt_4 = 0;    // 计时清零
      BUZZER_Middle(); // 蜂鸣器中速鸣叫
    }

    if (ms_cnt_2 >= 100) // 判断是否计时到100ms
    {
      ms_cnt_2 = 0; // 计时清零

      OLED_ClearArea(80, 16, 48, 16);
      OLED_Printf(80, 16, OLED_8X16, "%2.2fC", GET_NTC_Temperature()); // 显示NTC温度

      OLED_ClearArea(32, 0, 48, 32);
      OLED_ClearArea(40, 32, 48, 32);
      OLED_Printf(32, 0, OLED_8X16, "%2.2fV", ADC1_RESULT[0] * REF_3V3 / 16380.0 / (4.7 / 75.0));  // 显示输入电压
      OLED_Printf(32, 16, OLED_8X16, "%2.2fA", ADC1_RESULT[1] * REF_3V3 / 16380.0 / 62.0 / 0.005); // 显示输入电流
      OLED_Printf(40, 32, OLED_8X16, "%2.2fV", ADC1_RESULT[2] * REF_3V3 / 16380.0 / (4.7 / 75.0)); // 显示输出电压
      OLED_Printf(40, 48, OLED_8X16, "%2.2fA", ADC1_RESULT[3] * REF_3V3 / 16380.0 / 62.0 / 0.005); // 显示输出电流
      OLED_Update();                                                                               // 更新OLED显示内容

      float VIN = ADC1_RESULT[0] * 3.299 / 16380.0 / (4.7 / 75.0);
      float IIN = ADC1_RESULT[1] * 3.299 / 16380.0 / 62.0 / 0.005;
      float VOUT = ADC1_RESULT[2] * 3.299 / 16380.0 / (4.7 / 75.0);
      float IOUT = ADC1_RESULT[3] * 3.299 / 16380.0 / 62.0 / 0.005;
      float TEMP = GET_NTC_Temperature();
      float CPU_TEMP = GET_CPU_Temperature();
      USART1_Printf("%.3f,%.3f,%.3f,%.3f,%.2f,%.2f\n", VIN, IIN, VOUT, IOUT, TEMP, CPU_TEMP);
    }

    if (ms_cnt_1 >= 500) // 判断是否计时到500ms
    {
      ms_cnt_1 = 0;                                   // 计时清零
      HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin); // LED_R电平翻转
      // OLED_ShowFloatNum(48, 0, ADC1_RESULT[0] * 3.299 / 16380.0 / (4.7 / 75.0), 2, 3, OLED_8X16);  // 显示ADC1通道0采样结果
      // OLED_ShowFloatNum(48, 16, ADC1_RESULT[1] * 3.299 / 16380.0 / 62.0 / 0.005, 1, 3, OLED_8X16); // 显示ADC1通道1采样结果
      // OLED_ShowFloatNum(48, 32, ADC1_RESULT[2] * 3.299 / 16380.0 / (4.7 / 75.0), 2, 3, OLED_8X16); // 显示ADC1通道2采样结果
      // OLED_ShowFloatNum(48, 48, ADC1_RESULT[3] * 3.299 / 16380.0 / 62.0 / 0.005, 1, 3, OLED_8X16); // 显示ADC1通道3采样结果
      // OLED_UpdateArea(48, 0, 56, 63);                                                              // 更新OLED部分区域显示内容
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV5;
  RCC_OscInitStruct.PLL.PLLN = 68;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */

/**
 * @brief HAL_TIM_PeriodElapsedCallback函数,中断回调函数
 *
 * 当定时器周期结束时，该函数将被调用。
 *
 * @param htim TIM句柄指针
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM2) // 定时器TIM2，中断时间1ms
  {
    ms_cnt_1++;
    ms_cnt_2++;
    ms_cnt_3++;
    ms_cnt_4++;
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
