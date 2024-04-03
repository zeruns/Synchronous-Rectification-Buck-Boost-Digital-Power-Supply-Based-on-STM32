#include "PID.h"
#include "hrtim.h"
#include "function.h"

/*
 * 定义一个宏 CCMRAM，用于将函数或变量指定到CCM RAM段。
 * 使用此宏的声明将会被编译器放置在CCM（Cacheable Memory）RAM区域中。
 * 这对于需要快速访问且不被系统缓存机制影响的变量或函数非常有用。
 */
#define CCMRAM __attribute__((section("ccmram")))

extern struct _Ctr_value CtrValue;       // 控制参数
int32_t VErr0 = 0, VErr1 = 0, VErr2 = 0; // 电压误差
int32_t u0 = 0, u1 = 0;                  // 电压环输出量

/**
 * @brief BuckBoost电压电流环路控制PID函数。
 * 该函数用于实现BuckBoost电压电流环路控制的PID算法。
 * 在stm32g4xx_it.c文件中的HRTIM1_TIMD_IRQHandler中断函数里调用此函数。
 */
CCMRAM void BuckBoostVILoopCtlPID(void)
{
    int32_t VoutTemp = 0;                 // 输出电压
    VErr0 = CtrValue.Vout_ref - VoutTemp; // 计算电压误差量
}
