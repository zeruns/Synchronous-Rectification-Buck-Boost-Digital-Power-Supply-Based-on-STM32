#ifndef __KEY_H
#define __KEY_H

#include "main.h"

#define KEY_NUM 3 // 按键数量

#define Encoder_KEY HAL_GPIO_ReadPin(Encoder_KEY_GPIO_Port, Encoder_KEY_Pin)
#define KEY1 HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)
#define KEY2 HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)

extern uint8_t KEY_Status[KEY_NUM + 1][3]; // 记录各按键状态，按键数量+1跳过0号直观一点
extern uint8_t Key_Flag[KEY_NUM + 1];      // 按键标志位，记录各按键是否稳定按下，1表示按键已按下，0表示按键没被按下，按键数量+1跳过0号直观一点

void Key_Init(void);
void KEY_Scan(uint8_t key_num, uint8_t KEY);

#endif
