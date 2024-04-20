#ifndef __FUNCTION_H
#define __FUNCTION_H

#include "main.h"

/*
 * 定义一个宏 CCMRAM，用于将函数或变量指定到CCM RAM段。
 * 使用此宏的声明将会被编译器放置在CCM（Cacheable Memory）RAM区域中。
 * 这对于需要快速访问且不被系统缓存机制影响的变量或函数非常有用。
 */
#define CCMRAM __attribute__((section("ccmram")))

#define ADC_MAX_VALUE 8190.0F				   // ADC最大值
#define REF_3V3 3.2993F						   // VREF参考电压
#define TS_CAL1 *((__IO uint16_t *)0x1FFF75A8) // 内部温度传感器在30度和VREF为3V时的校准数据
#define TS_CAL2 *((__IO uint16_t *)0x1FFF75CA) // 内部温度传感器在130度和VREF为3V时的校准数据
#define TS_CAL1_TEMP 30.0F
#define TS_CAL2_TEMP 130.0F

#define MIN_BUKC_DUTY 100	  // BUCK最小占空比
#define MAX_BUCK_DUTY 28200	  // BUCK最大占空比94%
#define MAX_BUCK_DUTY1 24000  // MIX模式下 BUCK固定占空比80%
#define MIN_BOOST_DUTY 100	  // BOOST最小占空比
#define MIN_BOOST_DUTY1 1800  // BOOST最小占空6%
#define MAX_BOOST_DUTY 19500  // BOOST工作模式下最大占空比65%
#define MAX_BOOST_DUTY1 28200 // BOOST最大占空比94%

#define CAL_VOUT_K 4070 // 输出电压矫正K值
#define CAL_VOUT_B 3	// 输出电压矫正B值

/***************故障类型*****************/
#define F_NOERR 0x0000		 // 无故障
#define F_SW_VIN_UVP 0x0001	 // 输入欠压
#define F_SW_VIN_OVP 0x0002	 // 输入过压
#define F_SW_VOUT_UVP 0x0004 // 输出欠压
#define F_SW_VOUT_OVP 0x0008 // 输出过压
#define F_SW_IOUT_OCP 0x0010 // 输出过流
#define F_SW_SHORT 0x0020	 // 输出短路
#define F_OTP 0x0040		 // 温度过高

struct _SET_Value
{
	float SET_modified_flag; // 设置被修改标志位
	float Vout;				 // 输出电压设置值
	float Iout;				 // 输出电流设置值
	uint8_t currentSetting;	 // 当前设置项标志位，0表示没有选中设置项
	uint8_t SET_bit;		 // 当前设置位标志位，0表示没有选中设置位
};

// 控制参数结构体
struct _Ctr_value
{
	int32_t Vout_ref;	  // 输出参考电压
	int32_t Vout_SSref;	  // 软启动时的输出参考电压
	int32_t Iout_ref;	  // 输出参考电流
	int32_t I_Limit;	  // 限流参考电流
	int16_t BUCKMaxDuty;  // Buck最大占空比
	int16_t BoostMaxDuty; // Boost最大占空比
	int16_t BuckDuty;	  // Buck控制占空比
	int16_t BoostDuty;	  // Boost控制占空比
	int32_t Ilimitout;	  // 电流环输出
};

// 标志位定义
struct _FLAG
{
	uint16_t SMFlag;	  // 状态机标志位
	uint16_t CtrFlag;	  // 控制标志位
	uint16_t ErrFlag;	  // 故障标志位
	uint8_t BBFlag;		  // 运行模式标志位，BUCK模式，BOOST模式，MIX混合模式
	uint8_t PWMENFlag;	  // 启动标志位
	uint8_t BBModeChange; // 工作模式切换标志位
	uint8_t OUTPUT_Flag;  // 输出开关标志位, 0为关闭，1为开启
};

// 采样变量结构体
struct _ADI
{
	uint32_t Iout;	  // 输出电流
	uint32_t IoutAvg; // 输出电流平均值
	uint32_t Vout;	  // 输出电电压
	uint32_t VoutAvg; // 输出电电压平均值
	uint32_t Iin;	  // 输出电流
	uint32_t IinAvg;  // 输出电流平均值
	uint32_t Vin;	  // 输出电电压
	uint32_t VinAvg;  // 输出电电压平均值
};

// 状态机枚举量
typedef enum
{
	Init, // 初始化
	Wait, // 空闲等待
	Rise, // 软启
	Run,  // 正常运行
	Err	  // 故障
} STATE_M;

// 状态机枚举量
typedef enum
{
	NA,	   // 未定义
	Buck,  // BUCK模式
	Boost, // BOOST模式
	Mix	   // MIX混合模式
} BB_M;

// 软启动枚举变量
typedef enum
{
	SSInit, // 软启初始化
	SSWait, // 软启等待
	SSRun	// 开始软启
} SState_M;

// 屏幕页面枚举变量
typedef enum
{
	VIset_page = 1, // 电压电流设置页面
	DATA1_page,		// 数据显示页面1
	DATA2_page,		// 数据显示页面2
	SET_page		// 设置页面
} _Screen_page;

typedef enum
{
	CV, // 恒压模式
	CC	// 恒流模式
} _CVCC_Mode;

extern volatile uint16_t ADC1_RESULT[4];		// ADC1通道1~4采样结果
extern volatile uint8_t BUZZER_Short_Flag;		// 蜂鸣器短叫触发标志位
extern volatile uint8_t BUZZER_Flag;			// 蜂鸣器当前状态标志位
extern volatile uint8_t BUZZER_Middle_Flag;		// 蜂鸣器中等时间长度鸣叫触发标志位
extern struct _Ctr_value CtrValue;				// 控制参数
extern struct _FLAG DF;							// 控制标志位
extern struct _ADI SADC;						// 采样变量
extern volatile float VIN, VOUT, IIN, IOUT;		// 电压电流实际值
extern volatile float MainBoard_TEMP, CPU_TEMP; // 主板和CPU温度实际值
extern volatile float powerEfficiency;			// 电源转换效率
extern volatile _CVCC_Mode CVCC_Mode;			// 电源模式

/*
 * 设置寄存器的位
 * 参数：
 *   reg: 要操作的寄存器
 *   mask: 指定要设置的位掩码
 * 返回值：无
 */
#define setRegBits(reg, mask) (reg |= (unsigned int)(mask))

/*
 * 清除寄存器的位
 * 参数：
 *   reg: 要操作的寄存器
 *   mask: 指定要清除的位掩码
 * 返回值：无
 */
#define clrRegBits(reg, mask) (reg &= (unsigned int)(~(unsigned int)(mask)))

/*
 * 获取寄存器中指定位的值
 * 参数：
 *   reg: 要操作的寄存器
 *   mask: 指定要获取的位掩码
 * 返回值：掩码中为1的位的值
 */
#define getRegBits(reg, mask) (reg & (unsigned int)(mask))

/*
 * 获取寄存器的值
 * 参数：
 *   reg: 要获取的寄存器
 * 返回值：寄存器的当前值
 */
#define getReg(reg) (reg)

void ADCSample(void);
float GET_NTC_Temperature(void);
void Encoder(void);
void Key_Process(void);
void OLED_Display(void);
void ADC_calculate(void);
void StateM(void);
void StateMInit(void);
void StateMWait(void);
void StateMRise(void);
void StateMRun(void);
void StateMErr(void);
void ValInit(void);
void OTP(void);
void OVP(void);
void OCP(void);
void ShortOff(void);
void BBMode(void);
void BUZZER_Short(void);
void BUZZER_Middle(void);
float GET_CPU_Temperature(void);
void FAN_PWM_set(uint16_t pwm);

#endif
