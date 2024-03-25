#include "Key.h"

// 按键定义在.h文件中

extern uint8_t OUT_Status;
extern uint8_t OC_Status;

// 定义按键状态的枚举变量类型
typedef enum
{
	KS_RELEASE = 0, // 按键松开
	KS_SHAKE,		// 按键抖动
	KS_PRESS,		// 稳定按下
} KEY_STATUS;

// 当前循环结束的(状态机的)状态
#define g_keyStatus 0
// 当前状态(每次循环后与g_keyStatus保持一致)
#define g_nowKeyStatus 1
// 上次状态(用于记录前一状态以区分状态的来源)
#define g_lastKeyStatus 2

uint8_t KEY_Status[KEY_NUM + 1][3]; // 记录各按键状态，第一层数组表示按键编号，第二层数组表示按键各阶段状态，按键数量+1跳过0号直观一点
uint8_t Key_Flag[KEY_NUM + 1];		// 按键标志位，记录各按键是否稳定按下，1表示按键已按下，0表示按键没被按下，按键数量+1跳过0号直观一点
uint8_t Key_Flag_LOCK[KEY_NUM + 1]; // 按键锁定标志位，1表示按键被锁定，0表示按键未锁定，防止按键松开前反复触发按键事件

/**
 * @brief 初始化按键状态机。
 * 初始化按键状态机，将所有按键的状态都设置为松开状态。
 */
void Key_Init(void)
{
	uint8_t i;
	for (i = 0; i < KEY_NUM + 1; i++)
	{
		KEY_Status[i][g_keyStatus] = KS_RELEASE;
		KEY_Status[i][g_nowKeyStatus] = KS_RELEASE;
		KEY_Status[i][g_lastKeyStatus] = KS_RELEASE;
		Key_Flag[i] = 0;
		Key_Flag_LOCK[i] = 0;
	} // 按键状态机全部初始化为按键松开状态
}

/**
 * @brief 按键状态机扫描程序。
 * 根据按键编号和按键状态，扫描按键的状态并进行处理，10毫秒执行一次。
 * @param key_num 按键编号
 * @param KEY 按键当前状态，读取按键输入电平传入这里
 */
void KEY_Scan(uint8_t key_num, uint8_t KEY)
{
	switch (KEY_Status[key_num][g_keyStatus])
	{
	// 按键释放(初始状态)
	case KS_RELEASE:
	{
		// 检测到低电平，先进行消抖
		if (KEY == 0)
		{
			KEY_Status[key_num][g_keyStatus] = KS_SHAKE; // 抖动状态
		}
	}
	break;

	// 抖动
	case KS_SHAKE:
	{
		if (KEY == 1)
		{
			KEY_Status[key_num][g_keyStatus] = KS_RELEASE; // 松开状态
		}
		else
		{
			KEY_Status[key_num][g_keyStatus] = KS_PRESS; // 稳定按下状态
		}
	}
	break;

	// 稳定短按
	case KS_PRESS:
	{
		// 检测到高电平，先进行消抖
		if (KEY == 1)
		{
			KEY_Status[key_num][g_keyStatus] = KS_SHAKE; // 抖动状态
		}
		else
		{
			if (Key_Flag_LOCK[key_num] == 0)	
			{
				Key_Flag[key_num] = 1;		// 按键按下
				Key_Flag_LOCK[key_num] = 1; // 锁定按键状态
			}
		}
	}
	break;

	default:
		break;
	}

	if (KEY_Status[key_num][g_keyStatus] != KEY_Status[key_num][g_nowKeyStatus])	// 当前状态与前一次状态不一致
	{
		// 当前状态为松开 并且 前一次状态为按下
		if ((KEY_Status[key_num][g_keyStatus] == KS_RELEASE) && (KEY_Status[key_num][g_lastKeyStatus] == KS_PRESS)) // 检测按键状态是否松开
		{
			// Key_Flag[key_num] = 1;
			Key_Flag_LOCK[key_num] = 0; // 解锁按键状态
		}
		KEY_Status[key_num][g_lastKeyStatus] = KEY_Status[key_num][g_nowKeyStatus];	// 记录上一次状态
		KEY_Status[key_num][g_nowKeyStatus] = KEY_Status[key_num][g_keyStatus];		// 记录当前状态
	}
}
