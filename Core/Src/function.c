#include "function.h" //功能函数头文件
#include "OLED.h"
#include <math.h>
#include "adc.h"
#include "usart.h"
#include "tim.h"
#include "Key.h"
#include "hrtim.h"

volatile uint16_t ADC1_RESULT[4] = {0, 0, 0, 0};                       // ADC采样外设到内存的DMA数据保存寄存器
volatile uint8_t Encoder_Flag = 0;                                     // 编码器中断标志位
volatile uint8_t BUZZER_Short_Flag = 0;                                // 蜂鸣器短叫触发标志位
volatile uint8_t BUZZER_Middle_Flag = 0;                               // 蜂鸣器中等时间长度鸣叫触发标志位
volatile uint8_t BUZZER_Flag = 0;                                      // 蜂鸣器当前状态标志位
volatile float OTP_Value = 65.0;                                       // 过温保护阈值
CCMRAM struct _Ctr_value CtrValue = {0, 0, 0, MIN_BUKC_DUTY, 0, 0, 0}; // 控制参数
CCMRAM struct _FLAG DF = {0, 0, 0, 0, 0, 0};                           // 控制标志位
SState_M STState = SSInit;                                             // 软启动状态标志位

extern volatile int32_t VErr0, VErr1, VErr2; // 电压误差
extern volatile int32_t u0, u1;              // 电压环输出量

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
        BUZZER_Middle_Flag = 1; // 蜂鸣器中等时间长度鸣叫触发标志位置1

        USART1_Printf("按键1按下\r\n"); // 串口发送消息
        Key_Flag[1] = 0;                // 按键状态标志位清零
    }
    if (Key_Flag[2] == 1) // 如果按键2按下
    {
        BUZZER_Middle_Flag = 1; // 蜂鸣器中等时间长度鸣叫触发标志位置1
        if ((DF.SMFlag == Rise) || (DF.SMFlag == Run))
        {
            DF.SMFlag = Wait;
            // 关闭PWM
            DF.PWMENFlag = 0;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 关闭BUCK电路的PWM输出
            HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 关闭BOOST电路的PWM输出
        }
        else if (DF.SMFlag == Wait)
        {
            DF.SMFlag = Run;
            // 开闭PWM
            DF.PWMENFlag = 1;
            HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 开启HRTIM的PWM输出
            HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 开启HRTIM的PWM输出
        }
        USART1_Printf("按键2按下\r\n"); // 串口发送消息
        Key_Flag[2] = 0;                // 按键状态标志位清零
    }
    if (Key_Flag[3] == 1) // 如果编码器按键按下
    {
        BUZZER_Middle_Flag = 1; // 蜂鸣器中等时间长度鸣叫触发标志位置1

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
            OLED_ClearArea(88, 0, 48, 16);
            OLED_Printf(88, 0, OLED_8X16, "%d", encoder_num);
            OLED_Update();
        }
    }
}

/**
 * @brief OLED显示函数。
 * 刷新屏幕显示内容。
 */
void OLED_Display(void)
{
    OLED_Clear();
    OLED_ShowChinese(0, 0, "输入电压"); // 显示中文字
    OLED_ShowChinese(0, 16, "输入电流");
    OLED_ShowChinese(0, 32, "输出电压");
    OLED_ShowChinese(0, 48, "输出电流");
    OLED_ShowChar(64, 0, ':', OLED_8X16);                                                        // 显示冒号
    OLED_ShowChar(64, 16, ':', OLED_8X16);                                                       // 显示冒号
    OLED_ShowChar(64, 32, ':', OLED_8X16);                                                       // 显示冒号
    OLED_ShowChar(64, 48, ':', OLED_8X16);                                                       // 显示冒号
    OLED_Printf(72, 0, OLED_8X16, "%2.2fV", ADC1_RESULT[0] * REF_3V3 / 16380.0 / (4.7 / 75.0));  // 显示输入电压
    OLED_Printf(72, 16, OLED_8X16, "%2.2fA", ADC1_RESULT[1] * REF_3V3 / 16380.0 / 62.0 / 0.005); // 显示输入电流
    OLED_Printf(72, 32, OLED_8X16, "%2.2fV", ADC1_RESULT[2] * REF_3V3 / 16380.0 / (4.7 / 75.0)); // 显示输出电压
    OLED_Printf(72, 48, OLED_8X16, "%2.2fA", ADC1_RESULT[3] * REF_3V3 / 16380.0 / 62.0 / 0.005); // 显示输出电流
    OLED_Update();
}

/*
 * @brief 状态机函数，在5ms中断中运行，5ms运行一次
 */
void StateM(void)
{
    // 判断状态类型
    switch (DF.SMFlag)
    {
    // 初始化状态
    case Init:
        StateMInit();
        break;
    // 等待状态
    case Wait:
        StateMWait();
        break;
    // 软启动状态
    case Rise:
        StateMRise();
        break;
    // 运行状态
    case Run:
        StateMRun();
        break;
    // 故障状态
    case Err:
        StateMErr();
        break;
    }
}

/*
 * @brief 初始化状态函数，参数初始化
 */
void StateMInit(void)
{
    // 相关参数初始化
    ValInit();
    // 状态机跳转至等待软启状态
    DF.SMFlag = Wait;
}

/*
 * @brief 相关参数初始化函数
 */
void ValInit(void)
{
    // 关闭PWM
    DF.PWMENFlag = 0;
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 关闭BUCK电路的PWM输出
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 关闭BOOST电路的PWM输出
    // 清除故障标志位
    DF.ErrFlag = 0;
    // 初始化电压参考量
    CtrValue.Vout_ref = 0;
    // 限制占空比
    CtrValue.BuckDuty = MIN_BUKC_DUTY;
    CtrValue.BUCKMaxDuty = MIN_BUKC_DUTY;
    CtrValue.BoostDuty = MIN_BOOST_DUTY;
    CtrValue.BoostMaxDuty = MIN_BOOST_DUTY;
    // 环路计算变量初始化
    VErr0 = 0;
    VErr1 = 0;
    VErr2 = 0;
    u0 = 0;
    u1 = 0;
}

/*
 * @brief 正常运行，主处理函数在中断中运行
 */
void StateMRun(void)
{
}

/*
 * @brief 故障状态
 */
void StateMErr(void)
{
    // 关闭PWM
    DF.PWMENFlag = 0;
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 关闭BUCK电路的PWM输出
    HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 关闭BOOST电路的PWM输出
    // 若故障消除跳转至等待重新软启
    if (DF.ErrFlag == F_NOERR)
        DF.SMFlag = Wait;
}

/*
 * @brief 等待状态机
 */
void StateMWait(void)
{
}

#define MAX_SSCNT 20 // 等待100ms
/*
 * @brief 软启动阶段
 */
void StateMRise(void)
{
    // 计时器
    static uint16_t Cnt = 0;
    // 最大占空比限制计数器
    static uint16_t BUCKMaxDutyCnt = 0, BoostMaxDutyCnt = 0;

    // 判断软启状态
    switch (STState)
    {
    // 初始化状态
    case SSInit:
    {
        // 关闭PWM
        DF.PWMENFlag = 0;
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 关闭BUCK电路的PWM输出
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 关闭BOOST电路的PWM输出
        // 软启中将运行限制占空比启动，从最小占空比开始启动
        CtrValue.BUCKMaxDuty = MIN_BUKC_DUTY;
        CtrValue.BoostMaxDuty = MIN_BOOST_DUTY;
        // 环路计算变量初始化
        VErr0 = 0;
        VErr1 = 0;
        VErr2 = 0;
        u0 = 0;
        u1 = 0;
        // 跳转至软启等待状态
        STState = SSWait;

        break;
    }
    // 等待软启动状态
    case SSWait:
    {
        // 计数器累加
        Cnt++;
        // 等待100ms
        if (Cnt > MAX_SSCNT)
        {
            // 计数器清0
            Cnt = 0;
            // 限制启动占空比
            CtrValue.BuckDuty = MIN_BUKC_DUTY;
            CtrValue.BUCKMaxDuty = MIN_BUKC_DUTY;
            CtrValue.BoostDuty = MIN_BOOST_DUTY;
            CtrValue.BoostMaxDuty = MIN_BOOST_DUTY;
            // 环路计算变量初始化
            VErr0 = 0;
            VErr1 = 0;
            VErr2 = 0;
            u0 = 0;
            u1 = 0;
            // CtrValue.Voref输出参考电压从一半开始启动，避免过冲，然后缓慢上升
            CtrValue.Vout_ref = CtrValue.Vout_ref >> 1;
            STState = SSRun; // 跳转至软启状态
        }
        break;
    }
    // 软启动状态
    case SSRun:
    {
        if (DF.PWMENFlag == 0) // 正式发波前环路变量清0
        {
            // 环路计算变量初始化
            VErr0 = 0;
            VErr1 = 0;
            VErr2 = 0;
            u0 = 0;
            u1 = 0;
            HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 开启HRTIM的PWM输出
            HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 开启HRTIM的PWM输出
        }
        // 发波标志位置位
        DF.PWMENFlag = 1;
        // 最大占空比限制逐渐增加
        BUCKMaxDutyCnt++;
        BoostMaxDutyCnt++;
        // 最大占空比限制累加
        CtrValue.BUCKMaxDuty = CtrValue.BUCKMaxDuty + BUCKMaxDutyCnt * 5;
        CtrValue.BoostMaxDuty = CtrValue.BoostMaxDuty + BoostMaxDutyCnt * 5;
        // 累加到最大值
        if (CtrValue.BUCKMaxDuty > MAX_BUCK_DUTY)
            CtrValue.BUCKMaxDuty = MAX_BUCK_DUTY;
        if (CtrValue.BoostMaxDuty > MAX_BOOST_DUTY)
            CtrValue.BoostMaxDuty = MAX_BOOST_DUTY;

        if ((CtrValue.BUCKMaxDuty == MAX_BUCK_DUTY) && (CtrValue.BoostMaxDuty == MAX_BOOST_DUTY))
        {
            // 状态机跳转至运行状态
            DF.SMFlag = Run;
            // 软启动子状态跳转至初始化状态
            STState = SSInit;
        }
        break;
    }
    default:
        break;
    }
}

/**
 * @brief OTP 过温保护函数
 * OTP 函数用于处理温度过高的情况。
 * 函数需放5ms中断里执行。
 */
void OTP(void)
{
    float TEMP = GET_NTC_Temperature(); // 获取NTC温度值
    if (TEMP >= OTP_Value)
    {
        DF.SMFlag = Wait;
        // 关闭PWM
        DF.PWMENFlag = 0;
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 关闭BUCK电路的PWM输出
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 关闭BOOST电路的PWM输出
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
 * @brief 一阶低通滤波器。
 * 使用一阶低通滤波算法对输入信号进行滤波处理。
 * @param input 输入信号
 * @param alpha 滤波系数
 * @return 滤波后的输出信号
 */
float one_order_lowpass_filter(float input, float alpha)
{
    static float prev_output = 0.0;                           // 静态变量，用于保存上一次的输出值
    float output = alpha * input + (1 - alpha) * prev_output; // 一阶低通滤波算法
    prev_output = output;                                     // 保存本次输出值，以备下一次使用
    return output;                                            // 返回滤波后的输出信号
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

/**
 * @brief 读取CPU温度。
 * 使用ADC5采样单片机CPU温度，并根据校准值计算实际温度值。
 * @return 返回计算得到的温度值，单位为摄氏度。
 */
float GET_CPU_Temperature(void)
{
    HAL_ADC_Start(&hadc5); // 启动ADC5采样，采样单片机CPU温度
    // HAL_ADC_PollForConversion(&hadc5, 100); // 等待ADC采样结束
    float Temp_Scale = (float)(TS_CAL2_TEMP - TS_CAL1_TEMP) / (float)(TS_CAL2 - TS_CAL1); // 计算温度比例因子
    // 读取ADC5采样结果, 除以8是因为开启了硬件超采样到15bit，但下面计算用的是12bit，开启硬件超采样是为了得到一个比较平滑的采样结果
    float TEMP_adcValue = HAL_ADC_GetValue(&hadc5) / 8.0;
    float temperature = Temp_Scale * (TEMP_adcValue * (REF_3V3 / 3.0) - TS_CAL1) + TS_CAL1_TEMP; // 计算温度
    return temperature;                                                                          // 返回温度值
}

/**
 * @brief 设置风扇 PWM 值,
 * 根据给定的 PWM 值，设置风扇的 PWM 输出。
 * @param dutyCycle PWM 值，范围在 0 到 100 之间
 */
void FAN_PWM_set(uint16_t dutyCycle)
{
    if (dutyCycle > 100)
    {
        dutyCycle = 100;
    }
    __HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_3, dutyCycle * 10);
}
