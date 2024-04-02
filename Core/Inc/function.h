#ifndef __FUNCTION_H
#define __FUNCTION_H

#include "main.h"

#define REF_3V3 3.2993                         // VREF参考电压
#define TS_CAL1 *((__IO uint16_t *)0x1FFF75A8) // 内部温度传感器在30度和VREF为3V时的校准数据
#define TS_CAL2 *((__IO uint16_t *)0x1FFF75CA) // 内部温度传感器在130度和VREF为3V时的校准数据
#define TS_CAL1_TEMP 30.0
#define TS_CAL2_TEMP 130.0

extern volatile uint16_t ADC1_RESULT[4];    // ADC1通道1~4采样结果
extern volatile uint8_t BUZZER_Short_Flag;  // 蜂鸣器短叫触发标志位
extern volatile uint8_t BUZZER_Flag;        // 蜂鸣器当前状态标志位
extern volatile uint8_t BUZZER_Middle_Flag; // 蜂鸣器中等时间长度鸣叫触发标志位

// 控制参数结构体
struct  _Ctr_value
{
	int32_t		Voref;//参考电压
	int32_t		Ioref;//参考电流
	int32_t		ILimit;//限流参考电流
	int16_t		BUCKMaxDuty;//Buck最大占空比
	int16_t		BoostMaxDuty;//Boost最大占空比
	int16_t		BuckDuty;//Buck控制占空比
	int16_t		BoostDuty;//Boost控制占空比
	int32_t		Ilimitout;//电流环输出
};

float GET_NTC_Temperature(void);
void Encoder(void);
void Key_Process(void);
void BUZZER_Short(void);
void BUZZER_Middle(void);
float GET_CPU_Temperature(void);
void FAN_PWM_set(uint16_t pwm);

#endif
