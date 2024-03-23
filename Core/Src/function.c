#include "function.h" //功能函数头文件
#include "OLED.h"
#include <math.h>
#include "adc.h"

volatile uint16_t ADC1_RESULT[4] = {0, 0, 0, 0}; // ADC采样外设到内存的DMA数据保存寄存器

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
    uint32_t TEMP_adcValue = HAL_ADC_GetValue(&hadc2); // 读取ADC2采样结果
    float temperature = calculateTemperature(TEMP_adcValue * REF_3V3 / 65520.0); // 计算温度
    return temperature; // 返回温度值
}