#include "function.h" //功能函数头文件
#include "OLED.h"
#include <math.h>
#include "adc.h"
#include "usart.h"
#include "tim.h"
#include "Key.h"
#include "hrtim.h"

// 数字后面加F表示使用单精度浮点数类型，C语言默认使用双精度浮点数类型，硬件浮点运算只支持单精度浮点数

volatile uint16_t ADC1_RESULT[4] = {0, 0, 0, 0};                // ADC采样外设到内存的DMA数据保存寄存器
volatile uint8_t Encoder_Flag = 0;                              // 编码器中断标志位
volatile uint8_t BUZZER_Short_Flag = 0;                         // 蜂鸣器短叫触发标志位
volatile uint8_t BUZZER_Middle_Flag = 0;                        // 蜂鸣器中等时间长度鸣叫触发标志位
volatile uint8_t BUZZER_Flag = 0;                               // 蜂鸣器当前状态标志位
volatile float MAX_VOUT_OTP_VAL = 65.0F;                        // 过温保护阈值
volatile float MAX_VOUT_OVP_VAL = 50.0F;                        // 输出过压保护阈值
volatile float MAX_VOUT_OCP_VAL = 10.5F;                        // 输出过流保护阈值
#define MAX_SHORT_I 10.1F                                       // 短路电流判据
#define MIN_SHORT_V 0.5F                                        // 短路电压判据
struct _Ctr_value CtrValue = {0, 0, 0, MIN_BUKC_DUTY, 0, 0, 0}; // 控制参数
struct _FLAG DF = {0, 0, 0, 0, 0, 0, 0};                        // 控制标志位
struct _ADI SADC = {0, 0, 0, 0, 0, 0, 0, 0};                    // 输入输出参数采样值和平均值
struct _SET_Value SET_Value = {0, 0, 0, 0, 0};                  // 设置参数
SState_M STState = SSInit;                                      // 软启动状态标志位
_Screen_page Screen_page = VIset_page;                          // 当前屏幕页面标志位
volatile float VIN, VOUT, IIN, IOUT;                            // 电压电流实际值
volatile float MainBoard_TEMP, CPU_TEMP;                        // 主板和CPU温度实际值
volatile float powerEfficiency = 0;                             // 电源转换效率

extern volatile int32_t VErr0, VErr1, VErr2; // 电压误差
extern volatile int32_t u0, u1;              // 电压环输出量

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
    // 如果按键1按下
    if (Key_Flag[1] == 1)
    {
        BUZZER_Middle_Flag = 1; // 蜂鸣器中等时间长度鸣叫触发标志位置1
        if (DF.SMFlag == Err)   // 如果状态机处于错误状态
        {
            DF.ErrFlag = F_NOERR; // 消除故障状态
        }
        else if (Screen_page == VIset_page)
        {
            // 没有选中位时
            if (SET_Value.SET_bit == 0)
            {
                SET_Value.currentSetting++;       // 切换下一个设置项
                if (SET_Value.currentSetting > 2) // 如果超过最后一项，则回到第一项
                {
                    SET_Value.currentSetting = 0;
                }
            }
        }

        USART1_Printf("按键1按下\r\n"); // 串口发送消息
        Key_Flag[1] = 0;                // 按键状态标志位清零
    }
    // 如果按键2按下
    if (Key_Flag[2] == 1)
    {
        BUZZER_Middle_Flag = 1; // 蜂鸣器中等时间长度鸣叫触发标志位置1
        // 如果状态机处于错误状态
        if (DF.SMFlag == Err)
        {
            DF.ErrFlag = F_NOERR; // 消除故障状态
        }
        // 当状态机处于软启动状态或运行状态时
        else if ((DF.SMFlag == Rise) || (DF.SMFlag == Run))
        {
            DF.SMFlag = Wait;                                                            // 进入等待状态
            DF.OUTPUT_Flag = 0;                                                          // 输出关闭
            DF.PWMENFlag = 0;                                                            // 关闭PWM
            HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 关闭BUCK电路的PWM输出
            HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 关闭BOOST电路的PWM输出
        }
        // 当状态机处于等待状态时
        else if (DF.SMFlag == Wait)
        {
            DF.OUTPUT_Flag = 1; // 输出使能
            DF.SMFlag = Rise;   // 进入软启动状态
        }
        USART1_Printf("按键2按下\r\n"); // 串口发送消息
        Key_Flag[2] = 0;                // 按键状态标志位清零
    }
    // 如果编码器按键按下
    if (Key_Flag[3] == 1)
    {
        BUZZER_Middle_Flag = 1; // 蜂鸣器中等时间长度鸣叫触发标志位置1
        if (DF.SMFlag == Err)   // 如果状态机处于错误状态
        {
            DF.ErrFlag = F_NOERR; // 消除故障状态
        }
        // 当屏幕页面处于电压电流设置页面时
        else if (Screen_page == VIset_page)
        {
            // 当选中设置项时
            if (SET_Value.currentSetting != 0)
            {
                SET_Value.SET_bit++;       // 切换下一个设置位
                if (SET_Value.SET_bit > 4) // 如果超过最后一位，则回到没有选中位时
                {
                    SET_Value.SET_bit = 0;
                }
            }
        }

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
                USART1_Printf("编码器逆时针旋转\r\n");                     // 串口发送消息
                // 当没有选中设置项时
                if (SET_Value.currentSetting == 0)
                {
                    Screen_page--;                // 屏幕切换上一页
                    if (Screen_page < VIset_page) // 判断是否到首页
                    {
                        Screen_page = SET_page; // 切换到最后一页
                    }
                }
                // 当屏幕页面处于电压电流设置页面时
                if (Screen_page == VIset_page)
                {
                    // 选中电压设置时
                    if (SET_Value.currentSetting == 1)
                    {
                        // 选中十位时
                        if (SET_Value.SET_bit == 1)
                        {
                            SET_Value.Vout -= 10;
                            // 当设置值小于0.5时限位
                            if (SET_Value.Vout < 0.5)
                            {
                                SET_Value.Vout += 10;
                            }
                        }
                        // 选中个位时
                        else if (SET_Value.SET_bit == 2)
                        {
                            SET_Value.Vout -= 1;
                            // 当设置值小于0.5时限位
                            if (SET_Value.Vout < 0.5)
                            {
                                SET_Value.Vout = 0.5;
                            }
                        }
                        // 选中小数第一位时
                        else if (SET_Value.SET_bit == 3)
                        {
                            SET_Value.Vout -= 0.1;
                            // 当设置值小于0.5时限位
                            if (SET_Value.Vout < 0.5)
                            {
                                SET_Value.Vout += 0.1;
                            }
                        }
                        // 选中小数第二位时
                        else if (SET_Value.SET_bit == 4)
                        {
                            SET_Value.Vout -= 0.01;
                            // 当设置值小于0.5时限位
                            if (SET_Value.Vout < 0.5)
                            {
                                SET_Value.Vout += 0.01;
                            }
                        }
                        // 当设置被修改时
                        if (SET_Value.SET_bit != 0)
                        {
                            SET_Value.SET_modified_flag = 1; // 设置被修改标志位置1
                            // 将设置值传到参考值
                            CtrValue.Vout_ref = SET_Value.Vout * (4.7F / 75.0F) / REF_3V3 * ADC_MAX_VALUE;
                            CtrValue.Iout_ref = SET_Value.Iout * 0.005F * (6200.0F / 100.0F) / REF_3V3 * ADC_MAX_VALUE;
                        }
                    }
                    // 选中电流设置时
                    else if (SET_Value.currentSetting == 2)
                    {
                        // 选中十位时
                        if (SET_Value.SET_bit == 1)
                        {
                            SET_Value.Iout -= 10;
                            // 当设置值小于0.5时限位
                            if (SET_Value.Iout < 0.01)
                            {
                                SET_Value.Iout += 10;
                            }
                        }
                        // 选中个位时
                        else if (SET_Value.SET_bit == 2)
                        {
                            SET_Value.Iout -= 1;
                            // 当设置值小于0.5时限位
                            if (SET_Value.Iout < 0.01)
                            {
                                SET_Value.Iout = 0.01;
                            }
                        }
                        // 选中小数第一位时
                        else if (SET_Value.SET_bit == 3)
                        {
                            SET_Value.Iout -= 0.1;
                            // 当设置值小于0.5时限位
                            if (SET_Value.Iout < 0.01)
                            {
                                SET_Value.Iout += 0.1;
                            }
                        }
                        // 选中小数第二位时
                        else if (SET_Value.SET_bit == 4)
                        {
                            SET_Value.Iout -= 0.01;
                            // 当设置值小于0.5时限位
                            if (SET_Value.Iout < 0.01)
                            {
                                SET_Value.Iout += 0.01;
                            }
                        }
                        // 当设置被修改时
                        if (SET_Value.SET_bit != 0)
                        {
                            SET_Value.SET_modified_flag = 1; // 设置被修改标志位置1
                            // 将设置值传到参考值
                            CtrValue.Vout_ref = SET_Value.Vout * (4.7F / 75.0F) / REF_3V3 * ADC_MAX_VALUE;
                            CtrValue.Iout_ref = SET_Value.Iout * 0.005F * (6200.0F / 100.0F) / REF_3V3 * ADC_MAX_VALUE;
                        }
                    }
                }
            }
            else if (HAL_GPIO_ReadPin(Encoder_B_GPIO_Port, Encoder_B_Pin) == 0)
            {                                          // 编码器顺时针旋转
                USART1_Printf("编码器顺时针旋转\r\n"); // 串口发送消息
                // 当没有选中设置项时
                if (SET_Value.currentSetting == 0)
                {
                    Screen_page++;              // 屏幕切换下一页
                    if (Screen_page > SET_page) // 判断是否到最后一页
                    {
                        Screen_page = VIset_page; // 切换到首页
                    }
                }
                // 当屏幕页面处于电压电流设置页面时
                if (Screen_page == VIset_page)
                {
                    // 选中电压设置时
                    if (SET_Value.currentSetting == 1)
                    {
                        // 选中十位时
                        if (SET_Value.SET_bit == 1)
                        {
                            SET_Value.Vout += 10.0F;
                            // 当设置值大于48.5时限位
                            if (SET_Value.Vout > 48.5)
                            {
                                SET_Value.Vout -= 10.0F;
                            }
                        }
                        // 选中个位时
                        else if (SET_Value.SET_bit == 2)
                        {
                            SET_Value.Vout += 1;
                            // 当设置值大于48.5时限位
                            if (SET_Value.Vout > 48.5)
                            {
                                SET_Value.Vout = 48.5F;
                            }
                        }
                        // 选中小数第一位时
                        else if (SET_Value.SET_bit == 3)
                        {
                            SET_Value.Vout += 0.1F;
                            // 当设置值大于48.5时限位
                            if (SET_Value.Vout > 48.5)
                            {
                                SET_Value.Vout -= 0.1F;
                            }
                        }
                        // 选中小数第二位时
                        else if (SET_Value.SET_bit == 4)
                        {
                            SET_Value.Vout += 0.01F;
                            // 当设置值大于48.5时限位
                            if (SET_Value.Vout > 48.5)
                            {
                                SET_Value.Vout -= 0.01F;
                            }
                        }
                        // 当设置被修改时
                        if (SET_Value.SET_bit != 0)
                        {
                            SET_Value.SET_modified_flag = 1; // 设置被修改标志位置1

                            // 将设置值传到参考值
                            CtrValue.Vout_ref = SET_Value.Vout * (4.7F / 75.0F) / REF_3V3 * ADC_MAX_VALUE;
                            CtrValue.Iout_ref = SET_Value.Iout * 0.005F * (6200.0F / 100.0F) / REF_3V3 * ADC_MAX_VALUE;
                        }
                    }
                    // 选中电流设置时
                    else if (SET_Value.currentSetting == 2)
                    {
                        // 选中十位时
                        if (SET_Value.SET_bit == 1)
                        {
                            SET_Value.Iout += 10.0F;
                            // 当设置值大于48.5时限位
                            if (SET_Value.Iout > 10.0)
                            {
                                SET_Value.Iout -= 10.0F;
                            }
                        }
                        // 选中个位时
                        else if (SET_Value.SET_bit == 2)
                        {
                            SET_Value.Iout += 1;
                            // 当设置值大于48.5时限位
                            if (SET_Value.Iout > 10.0)
                            {
                                SET_Value.Iout = 10.0F;
                            }
                        }
                        // 选中小数第一位时
                        else if (SET_Value.SET_bit == 3)
                        {
                            SET_Value.Iout += 0.1F;
                            // 当设置值大于48.5时限位
                            if (SET_Value.Iout > 10.0)
                            {
                                SET_Value.Iout -= 0.1F;
                            }
                        }
                        // 选中小数第二位时
                        else if (SET_Value.SET_bit == 4)
                        {
                            SET_Value.Iout += 0.01F;
                            // 当设置值大于48.5时限位
                            if (SET_Value.Iout > 10.0)
                            {
                                SET_Value.Iout -= 0.01F;
                            }
                        }
                        // 当设置被修改时
                        if (SET_Value.SET_bit != 0)
                        {
                            SET_Value.SET_modified_flag = 1; // 设置被修改标志位置1

                            // 将设置值传到参考值
                            CtrValue.Vout_ref = SET_Value.Vout * (4.7F / 75.0F) / REF_3V3 * ADC_MAX_VALUE;
                            CtrValue.Iout_ref = SET_Value.Iout * 0.005F * (6200.0F / 100.0F) / REF_3V3 * ADC_MAX_VALUE;
                        }
                    }
                }
            }
            Encoder_Flag = 0;
        }
    }
}

/**
 * @brief OLED显示函数。
 * 刷新屏幕显示内容。
 */
void OLED_Display(void)
{
    if (DF.SMFlag != Err) // 非错误状态时
    {
        OLED_Clear();                  // 清除OLED屏显示缓冲区
        if (Screen_page == VIset_page) // 电压电流设置页面
        {
            OLED_ShowChinese(0, 0, "电压设置"); // 显示中文字
            OLED_ShowChinese(0, 16, "电流设置");
            OLED_ShowChinese(0, 32, "输出电压");
            OLED_ShowChinese(0, 48, "输出电流");
            OLED_ShowChar(64, 0, ':', OLED_8X16);                                                             // 显示冒号
            OLED_ShowChar(64, 16, ':', OLED_8X16);                                                            // 显示冒号
            OLED_ShowChar(64, 32, ':', OLED_8X16);                                                            // 显示冒号
            OLED_ShowChar(64, 48, ':', OLED_8X16);                                                            // 显示冒号
            OLED_ShowChar(72 + 8 * 5, 0, 'V', OLED_8X16);                                                     // 显示单位符号
            OLED_ShowChar(72 + 8 * 5, 16, 'A', OLED_8X16);                                                    // 显示单位符号
            OLED_ShowChar(72 + 8 * 5, 32, 'V', OLED_8X16);                                                    // 显示单位符号
            OLED_ShowChar(72 + 8 * 5, 48, 'A', OLED_8X16);                                                    // 显示单位符号
            OLED_ShowNum(72, 0, SET_Value.Vout + 0.005F, 2, OLED_8X16);                                       // 显示当前设置电压值整数部分
            OLED_ShowChar(72 + 8 * 2, 0, '.', OLED_8X16);                                                     // 显示小数点
            OLED_ShowNum(72 + 8 * 3, 0, (uint16_t)((SET_Value.Vout + 0.005F) * 100.0F) % 100, 2, OLED_8X16);  // 显示当前设置电压值小数部分,+0.005是为了四舍五入
            OLED_ShowNum(72, 16, SET_Value.Iout + 0.005F, 2, OLED_8X16);                                      // 显示当前设置电流值整数部分
            OLED_ShowChar(72 + 8 * 2, 16, '.', OLED_8X16);                                                    // 显示小数点
            OLED_ShowNum(72 + 8 * 3, 16, (uint16_t)((SET_Value.Iout + 0.005F) * 100.0F) % 100, 2, OLED_8X16); // 显示当前设置电流值小数部分,+0.005是为了四舍五入
            OLED_ShowNum(72, 32, VOUT + 0.005F, 2, OLED_8X16);                                                // 显示输出电压整数部分
            OLED_ShowChar(72 + 8 * 2, 32, '.', OLED_8X16);                                                    // 显示小数点
            OLED_ShowNum(72 + 8 * 3, 32, (uint16_t)((VOUT + 0.005F) * 100.0F) % 100, 2, OLED_8X16);           // 显示输出电压值小数部分,+0.005是为了四舍五入
            OLED_ShowNum(72, 48, IOUT + 0.005F, 2, OLED_8X16);                                                // 显示输出电流整数部分
            OLED_ShowChar(72 + 8 * 2, 48, '.', OLED_8X16);                                                    // 显示小数点
            OLED_ShowNum(72 + 8 * 3, 48, (uint16_t)((IOUT + 0.005F) * 100.0F) % 100, 2, OLED_8X16);           // 显示输出电流值小数部分,+0.005是为了四舍五入
            if (SET_Value.currentSetting == 1)                                                                // 选中第一个设置项时，输出电压设置
            {
                // 没有选中设置位时
                if (SET_Value.SET_bit == 0)
                {
                    OLED_ReverseArea(0, 0, 128, 16); // 反显当前设置项，输出电压设置
                }
                // 选中十位时
                else if (SET_Value.SET_bit == 1)
                {
                    OLED_ReverseArea(72, 0, 8, 16); // 反显当前设置位，十位
                }
                // 选中个位时
                else if (SET_Value.SET_bit == 2)
                {
                    OLED_ReverseArea(72 + 8 * 1, 0, 8, 16); // 反显当前设置位，个位
                }
                // 选中小数第一位时
                else if (SET_Value.SET_bit == 3)
                {
                    OLED_ReverseArea(72 + 8 * 3, 0, 8, 16); // 反显当前设置位，小数第一位
                }
                // 选中小数第二位时
                else if (SET_Value.SET_bit == 4)
                {
                    OLED_ReverseArea(72 + 8 * 4, 0, 8, 16); // 反显当前设置位，小数第二位
                }
            }
            else if (SET_Value.currentSetting == 2) // 选中第二个设置项时，输出电流设置
            {
                if (SET_Value.SET_bit == 0)
                {
                    OLED_ReverseArea(0, 16, 128, 16); // 反显当前设置项，输出电流设置
                }
                // 选中十位时
                else if (SET_Value.SET_bit == 1)
                {
                    OLED_ReverseArea(72, 16, 8, 16); // 反显当前设置位，十位
                }
                // 选中个位时
                else if (SET_Value.SET_bit == 2)
                {
                    OLED_ReverseArea(72 + 8 * 1, 16, 8, 16); // 反显当前设置位，个位
                }
                // 选中小数第一位时
                else if (SET_Value.SET_bit == 3)
                {
                    OLED_ReverseArea(72 + 8 * 3, 16, 8, 16); // 反显当前设置位，小数第一位
                }
                // 选中小数第二位时
                else if (SET_Value.SET_bit == 4)
                {
                    OLED_ReverseArea(72 + 8 * 4, 16, 8, 16); // 反显当前设置位，小数第二位
                }
            }
        }
        // 数据显示页面1
        else if (Screen_page == DATA1_page)
        {
            OLED_ShowChinese(0, 0, "输入电压"); // 显示中文字
            OLED_ShowChinese(0, 16, "输入电流");
            OLED_ShowChinese(0, 32, "输出电压");
            OLED_ShowChinese(0, 48, "输出电流");
            OLED_ShowChar(64, 0, ':', OLED_8X16);                                                   // 显示冒号
            OLED_ShowChar(64, 16, ':', OLED_8X16);                                                  // 显示冒号
            OLED_ShowChar(64, 32, ':', OLED_8X16);                                                  // 显示冒号
            OLED_ShowChar(64, 48, ':', OLED_8X16);                                                  // 显示冒号
            OLED_ShowChar(72 + 8 * 5, 0, 'V', OLED_8X16);                                           // 显示单位符号
            OLED_ShowChar(72 + 8 * 5, 16, 'A', OLED_8X16);                                          // 显示单位符号
            OLED_ShowChar(72 + 8 * 5, 32, 'V', OLED_8X16);                                          // 显示单位符号
            OLED_ShowChar(72 + 8 * 5, 48, 'A', OLED_8X16);                                          // 显示单位符号
            OLED_ShowNum(72, 0, VIN + 0.005F, 2, OLED_8X16);                                        // 显示输入电压值整数部分
            OLED_ShowChar(72 + 8 * 2, 0, '.', OLED_8X16);                                           // 显示小数点
            OLED_ShowNum(72 + 8 * 3, 0, (uint16_t)((VIN + 0.005F) * 100.0F) % 100, 2, OLED_8X16);   // 显示输入电压值小数部分,+0.005是为了四舍五入
            OLED_ShowNum(72, 16, IIN + 0.005F, 2, OLED_8X16);                                       // 显示输入电流值整数部分
            OLED_ShowChar(72 + 8 * 2, 16, '.', OLED_8X16);                                          // 显示小数点
            OLED_ShowNum(72 + 8 * 3, 16, (uint16_t)((IIN + 0.005F) * 100.0F) % 100, 2, OLED_8X16);  // 显示输入电流值小数部分,+0.005是为了四舍五入
            OLED_ShowNum(72, 32, VOUT + 0.005F, 2, OLED_8X16);                                      // 显示输出电压整数部分
            OLED_ShowChar(72 + 8 * 2, 32, '.', OLED_8X16);                                          // 显示小数点
            OLED_ShowNum(72 + 8 * 3, 32, (uint16_t)((VOUT + 0.005F) * 100.0F) % 100, 2, OLED_8X16); // 显示输出电压值小数部分,+0.005是为了四舍五入
            OLED_ShowNum(72, 48, IOUT + 0.005F, 2, OLED_8X16);                                      // 显示输出电流整数部分
            OLED_ShowChar(72 + 8 * 2, 48, '.', OLED_8X16);                                          // 显示小数点
            OLED_ShowNum(72 + 8 * 3, 48, (uint16_t)((IOUT + 0.005F) * 100.0F) % 100, 2, OLED_8X16); // 显示输出电流值小数部分,+0.005是为了四舍五入
        }
        else if (Screen_page == DATA2_page) // 数据显示页面2
        {
            OLED_ShowString(0, 0, "MCU", OLED_8X16);
            OLED_ShowChinese(24, 0, "温度"); // 显示中文字
            OLED_ShowChinese(0, 16, "主板温度");
            OLED_ShowChinese(0, 32, "转换效率");
            OLED_ShowChar(64, 0, ':', OLED_8X16);  // 显示冒号
            OLED_ShowChar(64, 16, ':', OLED_8X16); // 显示冒号
            OLED_ShowChar(64, 32, ':', OLED_8X16); // 显示冒号
            // OLED_ShowChar(64, 48, ':', OLED_8X16);                                              // 显示冒号
            OLED_ShowNum(72, 0, CPU_TEMP + 0.005F, 2, OLED_8X16);                                             // 显示CPU温度整数部分
            OLED_ShowChar(72 + 8 * 2, 0, '.', OLED_8X16);                                                     // 显示小数点
            OLED_ShowNum(72 + 8 * 3, 0, (uint16_t)((CPU_TEMP + 0.005F) * 100.0F) % 100, 2, OLED_8X16);        // 显示CPU温度小数部分，+0.005是为了四舍五入
            OLED_ShowChinese(72 + 8 * 5, 0, "℃");                                                             // 显示单位符号
            OLED_ShowNum(72, 16, MainBoard_TEMP + 0.005F, 2, OLED_8X16);                                      // 显示CPU温度整数部分
            OLED_ShowChar(72 + 8 * 2, 16, '.', OLED_8X16);                                                    // 显示小数点
            OLED_ShowNum(72 + 8 * 3, 16, (uint16_t)((MainBoard_TEMP + 0.005F) * 100.0F) % 100, 2, OLED_8X16); // 显示CPU温度小数部分，+0.005是为了四舍五入
            OLED_ShowChinese(72 + 8 * 5, 16, "℃");                                                            // 显示单位符号
            OLED_Printf(72, 32, OLED_8X16, "%2.2f%%", powerEfficiency);                                       // 显示转换效率
        }
        else if (Screen_page == SET_page) // 设置页面
        {
        }
        OLED_Update(); // 刷新屏幕显示
    }
    else
    {
        // 错误状态显示
        OLED_Clear();                              // 清除OLED屏显示缓冲区
        if (getRegBits(DF.ErrFlag, F_SW_VOUT_OVP)) // 判断是否输出过压保护状态
        {
            OLED_ShowChinese(32, 0, "输出过压");
        }
        if (getRegBits(DF.ErrFlag, F_SW_IOUT_OCP)) // 判断是否输出过流保护状态
        {
            OLED_ShowChinese(32, 16, "输出过流");
        }
        if (getRegBits(DF.ErrFlag, F_SW_SHORT)) // 判断是否短路保护状态
        {
            OLED_ShowChinese(32, 32, "输出短路");
        }
        OLED_Update();
    }
}

/**
 * @brief 采样输出电压、输出电流、输入电压、输入电流并滤波
 */
CCMRAM void ADCSample(void)
{
    // 输入输出采样参数求和，用以计算平均值
    static uint32_t VinAvgSum = 0, IinAvgSum = 0, VoutAvgSum = 0, IoutAvgSum = 0;

    // 从DMA缓冲器中获取数据
    SADC.Vin = (uint32_t)ADC1_RESULT[0];
    SADC.Iin = (uint32_t)ADC1_RESULT[1];
    SADC.Vout = (uint32_t)((ADC1_RESULT[2] * CAL_VOUT_K >> 12) + CAL_VOUT_B);
    SADC.Iout = (uint32_t)ADC1_RESULT[3];

    if (SADC.Vin < 15) // 采样有零偏离，采样值很小时，直接为0
        SADC.Vin = 0;
    if (SADC.Vout < 15)
        SADC.Vout = 0;
    if (SADC.Iout < 16)
        SADC.Iout = 0;

    // 计算各个采样值的平均值-滑动平均方式
    VinAvgSum = VinAvgSum + SADC.Vin - (VinAvgSum >> 3); // 求和，新增入一个新的采样值，同时减去之前的平均值。
    SADC.VinAvg = VinAvgSum >> 3;                        // 求平均
    IinAvgSum = IinAvgSum + SADC.Iin - (IinAvgSum >> 3);
    SADC.IinAvg = IinAvgSum >> 3;
    VoutAvgSum = VoutAvgSum + SADC.Vout - (VoutAvgSum >> 3);
    SADC.VoutAvg = VoutAvgSum >> 3;
    IoutAvgSum = IoutAvgSum + SADC.Iout - (IoutAvgSum >> 3);
    SADC.IoutAvg = IoutAvgSum >> 3;
}

/**
 * @brief ADC数据计算转换成实际数值的浮点数
 *
 */
void ADC_calculate(void)
{
    VIN = SADC.VinAvg * REF_3V3 / ADC_MAX_VALUE / (4.7F / 75.0F);   // 计算ADC1通道0输入电压采样结果
    IIN = SADC.IinAvg * REF_3V3 / ADC_MAX_VALUE / 62.0F / 0.005F;   // 计算ADC1通道1输入电流采样结果
    VOUT = SADC.VoutAvg * REF_3V3 / ADC_MAX_VALUE / (4.7F / 75.0F); // 计算ADC1通道2输出电压采样结果
    IOUT = SADC.IoutAvg * REF_3V3 / ADC_MAX_VALUE / 62.0F / 0.005F; // 计算ADC1通道3输出电流采样结果
    MainBoard_TEMP = GET_NTC_Temperature();                         // 获取NTC温度(主板温度)
    CPU_TEMP = GET_CPU_Temperature();                               // 获取单片机CPU温度
}

/*
 * @brief 状态机函数，在5ms中断中运行，5ms运行一次
 */
CCMRAM void StateM(void)
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
    // 设置值初始化
    SET_Value.Vout = 5.0;
    SET_Value.Iout = 1.0;
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
    // 计数器定义
    static uint16_t CntS = 0;
    static uint32_t IinSum = 0, IoutSum = 0;

    // 关PWM
    DF.PWMENFlag = 0;
    // 计数器累加
    CntS++;
    // 等待1S，进入启动状态
    if (CntS > 200)
    {
        CntS = 200;
        if ((DF.ErrFlag == F_NOERR) && (DF.OUTPUT_Flag == 1))
        {
            // 计数器清0
            CntS = 0;
            IinSum = 0;
            IoutSum = 0;
            // 状态标志位跳转至等待状态
            DF.SMFlag = Rise;
            // 软启动子状态跳转至初始化状态
            STState = SSInit;
        }
    }
}

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
        // 将设置值传到参考值
        CtrValue.Vout_ref = SET_Value.Vout * (4.7F / 75.0F) / REF_3V3 * ADC_MAX_VALUE;
        CtrValue.Iout_ref = SET_Value.Iout * 0.005F * (6200.0F / 100.0F) / REF_3V3 * ADC_MAX_VALUE;
        // 跳转至软启等待状态
        STState = SSWait;

        break;
    }
    // 等待软启动状态
    case SSWait:
    {
        // 计数器累加
        Cnt++;
        // 等待25ms
        if (Cnt > 5)
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
            // CtrValue.Vout_ref输出参考电压从一半开始启动，避免过冲，然后缓慢上升
            CtrValue.Vout_SSref = CtrValue.Vout_ref >> 1;
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
        CtrValue.BUCKMaxDuty = CtrValue.BUCKMaxDuty + BUCKMaxDutyCnt * 15;
        CtrValue.BoostMaxDuty = CtrValue.BoostMaxDuty + BoostMaxDutyCnt * 15;
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

void ShortOff(void)
{
    static int32_t RSCnt = 0;
    static uint8_t RSNum = 0;
    float Vout = SADC.Vout * REF_3V3 / ADC_MAX_VALUE / (4.7F / 75.0F);
    float Iout = SADC.Iout * REF_3V3 / ADC_MAX_VALUE / 62.0F / 0.005F;
    // 当输出电流大于 *A，且电压小于*V时，可判定为发生短路保护
    if ((Iout > MAX_SHORT_I) && (Vout < MIN_SHORT_V))
    {
        // 关闭PWM
        DF.PWMENFlag = 0;
        HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 开启HRTIM的PWM输出
        HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 开启HRTIM的PWM输出
        // 故障标志位
        setRegBits(DF.ErrFlag, F_SW_SHORT);
        // 跳转至故障状态
        DF.SMFlag = Err;
    }
    // 输出短路保护恢复
    // 当发生输出短路保护，关机后等待4S后清楚故障信息，进入等待状态等待重启
    if (getRegBits(DF.ErrFlag, F_SW_SHORT))
    {
        // 等待故障清楚计数器累加
        RSCnt++;
        // 等待2S
        if (RSCnt > 400)
        {
            // 计数器清零
            RSCnt = 0;
            // 短路重启只重启10次，10次后不重启
            if (RSNum > 10)
            {
                // 确保不清除故障，不重启
                RSNum = 11;
                // 关闭PWM
                DF.PWMENFlag = 0;
                HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 开启HRTIM的PWM输出
                HAL_HRTIM_WaveformOutputStart(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 开启HRTIM的PWM输出
            }
            else
            {
                // 短路重启计数器累加
                RSNum++;
                // 清除过流保护故障标志位
                clrRegBits(DF.ErrFlag, F_SW_SHORT);
            }
        }
    }
}

/**
 * @brief OVP 输出过压保护函数
 * OVP 函数用于处理输出电压过高的情况。
 * 函数需放5ms中断里执行。
 */
void OVP(void)
{
    // 过压保护判据保持计数器定义
    static uint16_t OVPCnt = 0;
    float Vout = SADC.Vout * REF_3V3 / ADC_MAX_VALUE / (4.7F / 75.0F);
    // 当输出电压大于50V，且保持10ms
    if (Vout >= MAX_VOUT_OVP_VAL)
    {
        // 条件保持计时
        OVPCnt++;
        // 条件保持10ms
        if (OVPCnt > 2)
        {
            // 计时器清零
            OVPCnt = 0;
            // 关闭PWM
            DF.PWMENFlag = 0;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 关闭BUCK电路的PWM输出
            HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 关闭BOOST电路的PWM输出
            // 故障标志位
            setRegBits(DF.ErrFlag, F_SW_VOUT_OVP);
            // 跳转至故障状态
            DF.SMFlag = Err;
        }
    }
    else
        OVPCnt = 0;
}

/**
 * @brief OCP 输出过流保护函数
 * OCP 函数用于处理输出电流过高的情况。
 * 函数需放5ms中断里执行。
 */
void OCP(void)
{
    // 过流保护判据保持计数器定义
    static uint16_t OCPCnt = 0;
    // 故障清楚保持计数器定义
    static uint16_t RSCnt = 0;
    // 保留保护重启计数器
    static uint16_t RSNum = 0;

    float Iout = SADC.Iout * REF_3V3 / ADC_MAX_VALUE / 62.0F / 0.005F;

    // 当输出电流大于*A，且保持50ms
    if ((Iout >= MAX_VOUT_OCP_VAL) && (DF.SMFlag == Run))
    {
        // 条件保持计时
        OCPCnt++;
        // 条件保持50ms，则认为过流发生
        if (OCPCnt > 10)
        {
            // 计数器清0
            OCPCnt = 0;
            // 关闭PWM
            DF.PWMENFlag = 0;
            HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 关闭BUCK电路的PWM输出
            HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 关闭BOOST电路的PWM输出
            // 故障标志位
            setRegBits(DF.ErrFlag, F_SW_IOUT_OCP);
            // 跳转至故障状态
            DF.SMFlag = Err;
        }
    }
    else
        // 计数器清0
        OCPCnt = 0;

    // 输出过流后恢复
    // 当发生输出软件过流保护，关机后等待4S后清楚故障信息，进入等待状态等待重启
    if (getRegBits(DF.ErrFlag, F_SW_IOUT_OCP))
    {
        // 等待故障清楚计数器累加
        RSCnt++;
        // 等待2S
        if (RSCnt > 400)
        {
            // 计数器清零
            RSCnt = 0;
            // 过流重启计数器累加
            RSNum++;
            // 过流重启只重启10次，10次后不重启（严重故障）
            if (RSNum > 10)
            {
                // 确保不清除故障，不重启
                RSNum = 11;
                // 关闭PWM
                DF.PWMENFlag = 0;
                HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 关闭BUCK电路的PWM输出
                HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 关闭BOOST电路的PWM输出
            }
            else
            {
                // 清除过流保护故障标志位
                clrRegBits(DF.ErrFlag, F_SW_IOUT_OCP);
            }
        }
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
    if (TEMP >= MAX_VOUT_OTP_VAL)
    {
        DF.SMFlag = Wait;
        // 关闭PWM
        DF.PWMENFlag = 0;
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2); // 关闭BUCK电路的PWM输出
        HAL_HRTIM_WaveformOutputStop(&hhrtim1, HRTIM_OUTPUT_TF1 | HRTIM_OUTPUT_TF2); // 关闭BOOST电路的PWM输出
    }
}

/**
 * @brief 运行模式判断。
 * BUCK模式：输出参考电压<0.8倍输入电压
 * BOOST模式：输出参考电压>1.2倍输入电压
 * MIX模式：1.15倍输入电压>输出参考电压>0.85倍输入电压
 * 当进入MIX（buck-boost）模式后，退出到BUCK或者BOOST时需要滞缓，防止在临界点来回振荡
 */
CCMRAM void BBMode(void)
{
    // 上一次模式状态量
    uint8_t PreBBFlag = 0;
    // 暂存当前的模式状态量
    PreBBFlag = DF.BBFlag;

    uint32_t VIN_ADC = ADC1_RESULT[0]; // 输入电压ADC采样值

    // 对输入电压ADC采样值累计取平均值
    static uint32_t VIN_ADC_SUM = 0;
    static uint8_t VIN_ADC_Count = 0;

    if (VIN_ADC_Count < 5)
    {
        VIN_ADC_SUM += ADC1_RESULT[0];
        VIN_ADC_Count++;
    }
    if (VIN_ADC_Count == 5)
    {
        VIN_ADC = VIN_ADC_SUM / 5;
        VIN_ADC_SUM = 0;
        VIN_ADC_Count = 0;
    }

    // 判断当前模块的工作模式
    switch (DF.BBFlag)
    {
    // NA-初始化模式
    case NA:
    {
        if (CtrValue.Vout_ref < (VIN_ADC * 0.8F))      // 输出参考电压小于0.8倍输入电压时
            DF.BBFlag = Buck;                          // 切换到buck模式
        else if (CtrValue.Vout_ref > (VIN_ADC * 1.2F)) // 输出参考电压大于1.2倍输入电压时
            DF.BBFlag = Boost;                         // 切换到boost模式
        else
            DF.BBFlag = Mix; // buck-boost（MIX） mode
        break;
    }
    // BUCK模式
    case Buck:
    {
        if (CtrValue.Vout_ref > (VIN_ADC * 1.2F))       // vout>1.2*vin
            DF.BBFlag = Boost;                          // boost mode
        else if (CtrValue.Vout_ref > (VIN_ADC * 0.85F)) // 1.2*vin>vout>0.85*vin
            DF.BBFlag = Mix;                            // buck-boost（MIX） mode
        break;
    }
    // Boost模式
    case Boost:
    {
        if (CtrValue.Vout_ref < ((VIN_ADC * 0.8F)))     // vout<0.8*vin
            DF.BBFlag = Buck;                           // buck mode
        else if (CtrValue.Vout_ref < (VIN_ADC * 1.15F)) // 0.8*vin<vout<1.15*vin
            DF.BBFlag = Mix;                            // buck-boost（MIX） mode
        break;
    }
    // Mix模式
    case Mix:
    {
        if (CtrValue.Vout_ref < (VIN_ADC * 0.8F))      // vout<0.8*vin
            DF.BBFlag = Buck;                          // buck mode
        else if (CtrValue.Vout_ref > (VIN_ADC * 1.2F)) // vout>1.2*vin
            DF.BBFlag = Boost;                         // boost mode
        break;
    }
    }

    // 当模式发生变换时（上一次和这一次不一样）,则标志位置位，标志位用以环路计算复位，保证模式切换过程不会有大的过冲
    if (PreBBFlag == DF.BBFlag)
        DF.BBModeChange = 0;
    else
        DF.BBModeChange = 1;
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
    static float prev_output = 0.0F;                             // 静态变量，用于保存上一次的输出值
    float output = alpha * input + (1.0F - alpha) * prev_output; // 一阶低通滤波算法
    prev_output = output;                                        // 保存本次输出值，以备下一次使用
    return output;                                               // 返回滤波后的输出信号
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
    float Rt = 0;                                                  // NTC电阻
    float R = 10000;                                               // 10K固定阻值电阻
    float T0 = 273.15F + 25;                                       // 转换为开尔文温度
    float B = 3950;                                                // B值
    float Ka = 273.15F;                                            // K值
    Rt = (REF_3V3 - voltage) * 10000.0F / voltage;                 // 计算Rt
    float temperature = 1.0F / (1.0F / T0 + log(Rt / R) / B) - Ka; // 计算温度
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
    uint32_t TEMP_adcValue = HAL_ADC_GetValue(&hadc2);                            // 读取ADC2采样结果
    float temperature = calculateTemperature(TEMP_adcValue * REF_3V3 / 65520.0F); // 计算温度
    return temperature;                                                           // 返回温度值
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
    float TEMP_adcValue = HAL_ADC_GetValue(&hadc5) / 8.0F;
    float temperature = Temp_Scale * (TEMP_adcValue * (REF_3V3 / 3.0F) - TS_CAL1) + TS_CAL1_TEMP; // 计算温度
    return one_order_lowpass_filter(temperature, 0.1F);                                           // 返回温度值
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
