#include "PID.h"
#include "hrtim.h"
#include "function.h"

/*
 * 定义一个宏 CCMRAM，用于将函数或变量指定到CCM RAM段。
 * 使用此宏的声明将会被编译器放置在CCM（Cacheable Memory）RAM区域中。
 * 这对于需要快速访问且不被系统缓存机制影响的变量或函数非常有用。
 */
#define CCMRAM __attribute__((section("ccmram")))

extern volatile uint16_t ADC1_RESULT[4];          // ADC1通道1~4采样结果
volatile int32_t VErr0 = 0, VErr1 = 0, VErr2 = 0; // 电压误差
volatile int32_t IErr0 = 0;                       // 电流误差
volatile int32_t u0 = 0, u1 = 0;                  // 电压环输出量
volatile int32_t i0 = 0;                          // 电流环输出量

void PID_Init(void)
{
    // 初始化PID参数
    // CtrValue.Vout_ref = Vset * (4.7 / 75.0) / REF_3V3 * 8190.0; // 输出电压参考值计算，设置电压的浮点数转换成ADC值的整数
    VErr0 = 0;
    VErr1 = 0;
    VErr2 = 0;
    u0 = 0;
    u1 = 0;
    i0 = 0;
    IErr0 = 0;
}

// 环路的参数buck输出-恒压-PID型补偿器
#define BUCKPIDb0 5271
#define BUCKPIDb1 -10363
#define BUCKPIDb2 5093
// 环路的参数BOOST输出-恒压-PID型补偿器
#define BOOSTPIDb0 8044
#define BOOSTPIDb1 -15813
#define BOOSTPIDb2 7772

#define ILOOP_KP     8//电流环PI环路P值
#define ILOOP_KI     2//电流环PI环路I值

/**
 * @brief BuckBoost电压电流环路控制PID函数。
 * 该函数用于实现BuckBoost电压电流环路控制的PID算法。
 * 在stm32g4xx_it.c文件中的HRTIM1_TIMD_IRQHandler中断函数里调用此函数。
 */
CCMRAM void BuckBoostVILoopCtlPID(void)
{
    // 电流环路积分量
    static int32_t I_Integral = 0;
    static int32_t IoutTemp = 0;

    int32_t VoutTemp = (ADC1_RESULT[2] * CAL_VOUT_K >> 12) + CAL_VOUT_B; // 获取矫正后的输出电压
    IoutTemp = (int32_t)ADC1_RESULT[3];                          // 获取输出电流

    if (DF.SMFlag == Rise)
    {
        VErr0 = CtrValue.Vout_SSref - VoutTemp; // 计算电压误差量
    }
    else
    {
        VErr0 = CtrValue.Vout_ref - VoutTemp; // 计算电压误差量，当参考电压大于输出电压，占空比增加，输出量增加
    }

    //计算电流误差量，当输出电流小于参考电流，输出量增加，慢速环，
	IErr0 = CtrValue.Iout_ref - IoutTemp;
	//电流环路输出=积分量+KP*误差量
	i0=I_Integral + IErr0*ILOOP_KP;
	//积分量=积分量+KI*误差量
	I_Integral = I_Integral+ IErr0*ILOOP_KI ;

    // 当模式切换时，降低占空比，确保模式切换不过冲
    // BBModeChange为模式切换为，不同模式切换时，该位会被置1
    if (DF.BBModeChange)
    {
        u1 = 0;
        DF.BBModeChange = 0;
    }

    // 判断工作模式，BUCK，BOOST，BUCK-BOOST
    switch (DF.BBFlag)
    {
    case NA: // 初始阶段
    {
        VErr0 = 0;
        VErr1 = 0;
        VErr2 = 0;
        u0 = 0;
        u1 = 0;
        i0 = 0;
        IErr0 = 0;
        break;
    }
    case Buck: // BUCK模式
    {
        u0 = u1 + VErr0 * BOOSTPIDb0 + VErr1 * BOOSTPIDb1 + VErr2 * BOOSTPIDb2;
        // 历史数据幅值
        VErr2 = VErr1;
        VErr1 = VErr0;
        u1 = u0;
        // 环路输出赋值
        CtrValue.BuckDuty = (u0 >> 8) * 3;
        CtrValue.BoostDuty = MIN_BOOST_DUTY1; // BOOST上管固定占空比94%，下管6%
        // 环路输出最大最小占空比限制
        if (CtrValue.BuckDuty > CtrValue.BUCKMaxDuty)
            CtrValue.BuckDuty = CtrValue.BUCKMaxDuty;
        if (CtrValue.BuckDuty < MIN_BUKC_DUTY)
            CtrValue.BuckDuty = MIN_BUKC_DUTY;
        break;
    }
    case Boost: // Boost模式
    {
        // 调用PID环路计算公式（参照PID环路计算文档）
        u0 = u1 + VErr0 * BOOSTPIDb0 + VErr1 * BOOSTPIDb1 + VErr2 * BOOSTPIDb2;
        // 历史数据幅值
        VErr2 = VErr1;
        VErr1 = VErr0;
        u1 = u0;
        // 环路输出赋值
        CtrValue.BuckDuty = MAX_BUCK_DUTY; // BUCK上管固定占空比94%
        CtrValue.BoostDuty = (u0 >> 8) * 3;
        // 环路输出最大最小占空比限制
        if (CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
            CtrValue.BoostDuty = CtrValue.BoostMaxDuty;
        if (CtrValue.BoostDuty < MIN_BOOST_DUTY)
            CtrValue.BoostDuty = MIN_BOOST_DUTY;
        break;
    }
    case Mix: // Mix模式
    {
        // 调用PID环路计算公式
        u0 = u1 + VErr0 * BOOSTPIDb0 + VErr1 * BOOSTPIDb1 + VErr2 * BOOSTPIDb2;
        // 历史数据幅值
        VErr2 = VErr1;
        VErr1 = VErr0;
        u1 = u0;
        // 环路输出赋值
        CtrValue.BuckDuty = MAX_BUCK_DUTY1; // BUCK上管固定占空比80%
        CtrValue.BoostDuty = (u0 >> 8) * 3;
        // 环路输出最大最小占空比限制
        if (CtrValue.BoostDuty > CtrValue.BoostMaxDuty)
            CtrValue.BoostDuty = CtrValue.BoostMaxDuty;
        if (CtrValue.BoostDuty < MIN_BOOST_DUTY)
            CtrValue.BoostDuty = MIN_BOOST_DUTY;
        break;
    }
    }

    // PWMENFlag是PWM开启标志位，当该位为0时,buck的占空比为0，无输出;
    if (DF.PWMENFlag == 0)
        CtrValue.BuckDuty = MIN_BUKC_DUTY;

    // 更新对应寄存器
    // buck占空比
    __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_COMPAREUNIT_1, PERIOD - CtrValue.BuckDuty);
    // ADC触发采样点，buck占空比的一半，右移1位为除以2
    __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_COMPAREUNIT_3, __HAL_HRTIM_GETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, HRTIM_COMPAREUNIT_1) >> 1);
    // Boost占空比
    __HAL_HRTIM_SETCOMPARE(&hhrtim1, HRTIM_TIMERINDEX_TIMER_F, HRTIM_COMPAREUNIT_1, CtrValue.BoostDuty);
}
