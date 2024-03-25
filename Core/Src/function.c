#include "function.h" //功能函数头文件
#include "OLED.h"
#include <math.h>
#include "adc.h"
#include "usart.h"
#include "Key.h"

volatile uint16_t ADC1_RESULT[4] = {0, 0, 0, 0}; // ADC采样外设到内存的DMA数据保存寄存器
volatile uint8_t Encoder_Flag = 0;               // 编码器中断标志位
volatile uint8_t BUZZER_Short_Flag = 0;          // 蜂鸣器短叫触发标志位
volatile uint8_t BUZZER_Middle_Flag = 0;         // 蜂鸣器中等时间长度鸣叫触发标志位
volatile uint8_t BUZZER_Flag = 0;                // 蜂鸣器当前状态标志位
volatile int16_t encoder_num = 0;

/**
 * @brief GPIO外部中断回调函数。
 * 当指定的 GPIO 引脚发生外部中断事件时，该函数将被调用。
 * @param GPIO_Pin GPIO 引脚号
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == Encoder_A_Pin)
    {
        Encoder_Flag = 1;
    }
}

/**
 * @brief 按键按下的处理函数。
 */
void Key_Process(void)
{
    if (Key_Flag[1] == 1) // 如果按键1按下
    {
        BUZZER_Middle_Flag = 1;         // 蜂鸣器中等时间长度鸣叫触发标志位置1
        
        USART1_Printf("按键1按下\r\n"); // 串口发送消息
        Key_Flag[1] = 0;                // 按键状态标志位清零
    }
    if (Key_Flag[2] == 1) // 如果按键2按下
    {
        BUZZER_Middle_Flag = 1;         // 蜂鸣器中等时间长度鸣叫触发标志位置1
        
        USART1_Printf("按键2按下\r\n"); // 串口发送消息
        Key_Flag[2] = 0;                // 按键状态标志位清零
    }
    if (Key_Flag[3] == 1) // 如果编码器按键按下
    {
        BUZZER_Middle_Flag = 1;              // 蜂鸣器中等时间长度鸣叫触发标志位置1
        
        USART1_Printf("编码器按键按下\r\n"); // 串口发送消息  
        Key_Flag[3] = 0;                     // 按键状态标志位清零
    }
}

/**
 * @brief 编码器处理函数。
 * 如果编码器标志位为1，则执行编码器相关操作。
 */
void Encoder(void)
{
    if (Encoder_Flag == 1)
    {
        HAL_Delay(1); // 延时1ms,消抖
        if (HAL_GPIO_ReadPin(Encoder_A_GPIO_Port, Encoder_A_Pin) == 0)
        {
            BUZZER_Short_Flag = 1;                                         // 蜂鸣器短促声触发标志位置1
            if (HAL_GPIO_ReadPin(Encoder_B_GPIO_Port, Encoder_B_Pin) == 1) // 编码器A相比B相提前
            {                                                              // 编码器逆时针旋转
                USART1_Printf("编码器逆时针旋转\r\n");
                encoder_num--;
            }
            else if (HAL_GPIO_ReadPin(Encoder_B_GPIO_Port, Encoder_B_Pin) == 0)
            { // 编码器顺时针旋转
                USART1_Printf("编码器顺时针旋转\r\n");
                encoder_num++;
            }
            Encoder_Flag = 0;
            OLED_ClearArea(80, 0, 48, 16);
            OLED_Printf(80, 0, OLED_8X16, "%d", encoder_num);
            OLED_Update();
        }
    }
}

/**
 * @brief 蜂鸣器短促声。
 * 根据 BUZZER_Flag 和 BUZZER_Short_Flag 的状态，控制蜂鸣器发出短促的声音。
 * 需10毫秒间隔执行一次。
 */
void BUZZER_Short(void)
{
    if (BUZZER_Flag == 1 && BUZZER_Middle_Flag == 0)
    {
        HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET); // 关闭蜂鸣器
        BUZZER_Flag = 0;                                                 // 蜂鸣器当前状态标志位置为0
        BUZZER_Short_Flag = 0;                                           // 蜂鸣器短叫触发标志位置为0
    }
    else if (BUZZER_Short_Flag == 1)
    {
        HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET); // 开启蜂鸣器
        BUZZER_Flag = 1;                                               // 蜂鸣器当前状态标志位置为1
    }
}

/**
 * @brief 蜂鸣器中等时间长度的鸣叫。
 * 根据 BUZZER_Flag 和 BUZZER_Middle_Flag 的状态，控制蜂鸣器发出中等时间长度的鸣叫声音。
 * 需50毫秒间隔执行一次。
 */
void BUZZER_Middle(void)
{
    if (BUZZER_Flag == 1)
    {
        HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET); // 关闭蜂鸣器
        BUZZER_Flag = 0;                                                 // 蜂鸣器当前状态标志位置为0
        BUZZER_Middle_Flag = 0;                                          // 蜂鸣器短叫触发标志位置为0
    }
    else if (BUZZER_Middle_Flag == 1)
    {
        HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET); // 开启蜂鸣器
        BUZZER_Flag = 1;                                               // 蜂鸣器当前状态标志位置为1
    }
}

/**
 * @brief 计算NTC温度,
 * 根据给定的电阻值计算温度。
 * @param resistance 电阻值
 * @return 计算得到的温度值
 */
float calculateTemperature(float voltage)
{
    // 数据进入前，可先做滤波处理
    float Rt = 0;                                            // NTC电阻
    float R = 10000;                                         // 10K固定阻值电阻
    float T0 = 273.15 + 25;                                  // 转换为开尔文温度
    float B = 3950;                                          // B值
    float Ka = 273.15;                                       // K值
    Rt = (REF_3V3 - voltage) * 10000 / voltage;              // 计算Rt
    float temperature = 1 / (1 / T0 + log(Rt / R) / B) - Ka; // 计算温度
    return temperature;
}

/**
 * @brief 获取NTC温度
 * @return 返回温度值
 */
float GET_NTC_Temperature(void)
{
    HAL_ADC_Start(&hadc2); // 启动ADC2采样，采样NTC温度
    // HAL_ADC_PollForConversion(&hadc2, 100); // 等待ADC采样结束
    uint32_t TEMP_adcValue = HAL_ADC_GetValue(&hadc2);                           // 读取ADC2采样结果
    float temperature = calculateTemperature(TEMP_adcValue * REF_3V3 / 65520.0); // 计算温度
    return temperature;                                                          // 返回温度值
}
