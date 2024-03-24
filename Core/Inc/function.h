#ifndef __FUNCTION_H
#define __FUNCTION_H

#include "main.h"

extern volatile uint16_t ADC1_RESULT[4];

#define REF_3V3 3.2993 // VREF参考电压

float GET_NTC_Temperature(void);

#endif
