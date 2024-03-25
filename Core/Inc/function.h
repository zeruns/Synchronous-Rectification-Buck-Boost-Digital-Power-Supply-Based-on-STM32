#ifndef __FUNCTION_H
#define __FUNCTION_H

#include "main.h"

#define REF_3V3 3.2993 // VREF参考电压

extern volatile uint16_t ADC1_RESULT[4];        // ADC1通道1~4采样结果
extern volatile uint8_t BUZZER_Short_Flag;      // 蜂鸣器短叫触发标志位
extern volatile uint8_t BUZZER_Flag;            // 蜂鸣器当前状态标志位
extern volatile uint8_t BUZZER_Middle_Flag; // 蜂鸣器中等时间长度鸣叫触发标志位

float GET_NTC_Temperature(void);
void Encoder(void);
void Key_Process(void);
void BUZZER_Short(void);
void BUZZER_Middle(void);

#endif
