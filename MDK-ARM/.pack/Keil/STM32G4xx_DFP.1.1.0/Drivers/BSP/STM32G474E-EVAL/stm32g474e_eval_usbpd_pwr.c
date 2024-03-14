/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g474e_discovery_usbpd_pwr.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the Type-C
  *          Power Delivery (PD):
  *            - VBUS control
  *            - VBUS voltage/current measurement
  *            - VCONN control
  *            - VBUS presence detection
  ******************************************************************************
  *
  * Copyright (c) 2018 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                               www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_usbpd_pwr.h"

/* USER CODE BEGIN include */
#include "stm32g4xx_ll_bus.h"
#include "stm32g4xx_ll_gpio.h"
/* USER CODE END include */

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @addtogroup STM32G474E-EVAL_USBPD_POWER
  * @{
  */

/** @defgroup STM32G474E_EVAL_USBPD_PWR_Private_Typedef Private Typedef
  * @{
  */
/* USER CODE BEGIN POWER_Private_Typedef */
/* USER CODE END POWER_Private_Typedef */
/**
  * @}
  */

/** @defgroup STM32G474E_EVAL_USBPD_PWR_Private_Constants Private Constants
* @{
*/
/* USER CODE BEGIN POWER_Private_Constants */
/* On EVAL STM32G474E_EVAL MB1397 REVA used for G474 validation, following setup has been used :
      VSENSE      => PC0
      SOURCE_EN   => PC11
      DISCHARGE   => PB2  */
#define VSENSE_GPIO_PORT                GPIOC
#define VSENSE_GPIO_PIN                 LL_GPIO_PIN_0
#define VSENSE_GPIO_ENABLE_CLOCK()      LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

#define SOURCE_EN_GPIO_PORT             GPIOC
#define SOURCE_EN_GPIO_PIN              LL_GPIO_PIN_11
#define SOURCE_EN_GPIO_ENABLE_CLOCK()   LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
#define SOURCE_EN_SET_OFF()             LL_GPIO_ResetOutputPin(SOURCE_EN_GPIO_PORT, SOURCE_EN_GPIO_PIN);
#define SOURCE_EN_SET_ON()              LL_GPIO_SetOutputPin(SOURCE_EN_GPIO_PORT, SOURCE_EN_GPIO_PIN);
#define IS_SOURCE_EN_SET_ON()           (LL_GPIO_IsOutputPinSet(SOURCE_EN_GPIO_PORT, SOURCE_EN_GPIO_PIN) == 1)

#define DISCHARGE_GPIO_PORT             GPIOB
#define DISCHARGE_GPIO_PIN              LL_GPIO_PIN_2
#define DISCHARGE_GPIO_ENABLE_CLOCK()   LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
#define DISCHARGE_SET_OFF()             LL_GPIO_ResetOutputPin(DISCHARGE_GPIO_PORT, DISCHARGE_GPIO_PIN);
#define DISCHARGE_SET_ON()              LL_GPIO_SetOutputPin(DISCHARGE_GPIO_PORT, DISCHARGE_GPIO_PIN);

/* PC0 is used as ADC12_IN6 input for ADC measurement of VBUS voltage :
   ADC1 (Common 12) Channel 1.
*/
#define VSENSE_ADC_INSTANCE             ADC1
#define VSENSE_ADC_COMMON               ADC12_COMMON
#define VSENSE_ADC_RANK                 LL_ADC_REG_RANK_1
#define VSENSE_ADC_CHANNEL              LL_ADC_CHANNEL_6

/* Enable ADC clock (core clock) */
#define VSENSE_ADC_ENABLE_CLOCK()    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);

/* Enable ADC clock (core clock) */
#define VSENSE_ADC_ENABLE_CLOCK()       LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);

/* Delay between ADC end of calibration and ADC enable.                     */
/* Delay estimation in CPU cycles: Case of ADC enable done                  */
/* immediately after ADC calibration, ADC clock setting slow                */
/* (LL_ADC_CLOCK_ASYNC_DIV32). Use a higher delay if ratio                  */
/* (CPU clock / ADC clock) is above 32.                                     */

#define ADC_DELAY_CALIB_ENABLE_CPU_CYCLES  (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 32)

#define VDDA_APPLI            3300U
/* USER CODE END POWER_Private_Constants */
/**
  * @}
  */

/** @defgroup STM32G474E_EVAL_USBPD_PWR_Private_Macros Private Macros
  * @{
  */
/* USER CODE BEGIN POWER_Private_Macros */
/* USER CODE END POWER_Private_Macros */
/**
  * @}
  */

/** @defgroup STM32G474E_EVAL_USBPD_PWR_Private_Variables Private Variables
  * @{
  */
/* USER CODE BEGIN POWER_Private_Variables */
/* USER CODE END POWER_Private_Variables */
/**
  * @}
  */

/** @defgroup STM32G474E_EVAL_USBPD_PWR_Private_Functions Private Functions
  * @{
  */
/* USER CODE BEGIN POWER_Private_Prototypes */
static void     PWR_VBUSDischarge(uint32_t PortId,
                           uint32_t VbusStopInmV,
                           uint32_t VbusErrorInmV,
                           uint16_t Delay);
static void PWR_Configure_ADC(void);
static void PWR_Activate_ADC(void);
/* USER CODE END POWER_Private_Prototypes */
/**
  * @}
  */

/** @defgroup STM32G474E_EVAL_USBPD_PWR_Exported_Variables Exported Variables
  * @{
  */
/* USER CODE BEGIN POWER_Exported_Variables */
/* USER CODE END POWER_Exported_Variables */
/**
  * @}
  */

/** @addtogroup STM32G474E_EVAL_USBPD_PWR_Exported_Functions
  * @{
  */

/**
  * @brief  Initialize the hardware resources used by the Type-C power delivery (PD)
  *         controller.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VBUSInit(uint32_t               PortId)
{
/* USER CODE BEGIN  BSP_PWR_VBUSInit */
  /* For Sink cases */
  PWR_Configure_ADC();
  PWR_Activate_ADC();
  /*  MeasureVrefAnalog(); */
  LL_ADC_REG_StartConversion(VSENSE_ADC_INSTANCE);

  /* Enable the peripheral clock of GPIO Port for SOURCE_EN */
  SOURCE_EN_GPIO_ENABLE_CLOCK();

  /* For SRC cases VBUS On/OFF */
  LL_GPIO_SetPinMode(SOURCE_EN_GPIO_PORT, SOURCE_EN_GPIO_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(SOURCE_EN_GPIO_PORT, SOURCE_EN_GPIO_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(SOURCE_EN_GPIO_PORT, SOURCE_EN_GPIO_PIN, LL_GPIO_PULL_NO);
  /* Open path for USBPD_VBUS to to USB_PD_VBUS */
  SOURCE_EN_SET_OFF();

  /* Enable the peripheral clock of GPIO Port for DISCHARGE */
  DISCHARGE_GPIO_ENABLE_CLOCK();

  /* VBUS Discharge */
  LL_GPIO_SetPinMode(DISCHARGE_GPIO_PORT, DISCHARGE_GPIO_PIN, LL_GPIO_MODE_OUTPUT);
  LL_GPIO_SetPinOutputType(DISCHARGE_GPIO_PORT, DISCHARGE_GPIO_PIN, LL_GPIO_OUTPUT_PUSHPULL);
  LL_GPIO_SetPinPull(DISCHARGE_GPIO_PORT, DISCHARGE_GPIO_PIN, LL_GPIO_PULL_DOWN);

  /* Need to keep DISCHARGE ON as soon as not powering VBUS on MB1397 */
  DISCHARGE_SET_ON();

  return PWR_OK;
/* USER CODE END  BSP_PWR_VBUSInit */
}

/**
  * @brief  Release the hardware resources used by the Type-C power delivery (PD)
  *         controller.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VBUSDeInit(uint32_t PortId)
{
/* USER CODE BEGIN  BSP_PWR_VBUSDeInit */
  return PWR_ERROR;
/* USER CODE END  BSP_PWR_VBUSDeInit */
}

/**
  * @brief  Enable power supply over VBUS.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VBUSOn(uint32_t PortId)
{
/* USER CODE BEGIN  BSP_PWR_VBUSOn */
  DISCHARGE_SET_OFF();
  SOURCE_EN_SET_ON();

  return PWR_OK;
/* USER CODE END  BSP_PWR_VBUSOn */
}

/**
  * @brief  Disable power supply over VBUS.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VBUSOff(uint32_t PortId)
{
/* USER CODE BEGIN BSP_PWR_VBUSOff */
  /* switch off VBUS */
  SOURCE_EN_SET_OFF();

  /* Discharge VBUS */
  PWR_VBUSDischarge(PortId,  BSP_PWR_LOW_VBUS_THRESHOLD, 0 , 0);

  return PWR_OK;
/* USER CODE END BSP_PWR_VBUSOff */
}

/**
  * @brief  Set a fixed/variable PDO and manage the power control.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  * @param  VbusTargetInmv the vbus Target (in mV)
  * @param  OperatingCurrent the Operating Current (in mA)
  * @param  MaxOperatingCurrent the Max Operating Current (in mA)
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_Fixed(uint32_t PortId,
                                               uint32_t VbusTargetInmv,
                                               uint32_t OperatingCurrent,
                                               uint32_t MaxOperatingCurrent)
{
/* USER CODE BEGIN  BSP_PWR_VBUSSetVoltage_Fixed */
  return PWR_OK;
/* USER CODE END  BSP_PWR_VBUSSetVoltage_Fixed */
}

/**
  * @brief  Set a fixed/variable PDO and manage the power control.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  * @param  VbusTargetMinInmv the vbus Target min (in mV)
  * @param  VbusTargetMaxInmv the vbus Target max (in mV)
  * @param  OperatingCurrent the Operating Current (in mA)
  * @param  MaxOperatingCurrent the Max Operating Current (in mA)
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_Variable(uint32_t PortId,
                                                  uint32_t VbusTargetMinInmv,
                                                  uint32_t VbusTargetMaxInmv,
                                                  uint32_t OperatingCurrent,
                                                  uint32_t MaxOperatingCurrent)
{
/* USER CODE BEGIN  BSP_PWR_VBUSSetVoltage_Variable */
  return PWR_ERROR;
/* USER CODE BEGIN  BSP_PWR_VBUSSetVoltage_Variable */
}

/**
  * @brief  Set a Battery PDO and manage the power control.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  * @param  VbusTargetMin the vbus Target min (in mV)
  * @param  VbusTargetMax the vbus Target max (in mV)
  * @param  OperatingPower the Operating Power (in mW)
  * @param  MaxOperatingPower the Max Operating Power (in mW)
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_Battery(uint32_t PortId,
                                                 uint32_t VbusTargetMin,
                                                 uint32_t VbusTargetMax,
                                                 uint32_t OperatingPower,
                                                 uint32_t MaxOperatingPower)
{
/* USER CODE BEGIN  BSP_PWR_VBUSSetVoltage_Battery */
  return PWR_ERROR;
/* USER CODE BEGIN  BSP_PWR_VBUSSetVoltage_Battery */
}

/**
  * @brief  Set a APDO and manage the power control.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  * @param  VbusTargetInmv the vbus Target (in mV)
  * @param  OperatingCurrent the Operating current (in mA)
  * @param  Delta Delta between with previous APDO (in mV), 0 means APDO start
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VBUSSetVoltage_APDO(uint32_t PortId,
                                              uint32_t VbusTargetInmv,
                                              uint32_t OperatingCurrent,
                                              int32_t Delta)
{
/* USER CODE BEGIN  BSP_PWR_VBUSSetVoltage_APDO */
  return PWR_ERROR;
/* USER CODE BEGIN  BSP_PWR_VBUSSetVoltage_APDO */
}

/**
  * @brief  Get actual voltage level measured on the VBUS line.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
 * @retval Voltage measured voltage level (in mV)
  */
uint32_t  BSP_PWR_VBUSGetVoltage(uint32_t PortId)
{
/* USER CODE BEGIN  BSP_PWR_VBUSGetVoltage */
  uint32_t val;

  val = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI,  LL_ADC_REG_ReadConversionData12(VSENSE_ADC_INSTANCE), LL_ADC_RESOLUTION_12B); /* mV */

  /* STM32G474E_EVAL board is used */
  /* Value is multiplied by 7.030 according to board measurments.
     Theorically, it should have been 7.613 (Divider R323/R244 (49.9K/330K) for VSENSE */
  val *= 7030;
  val /= 1000;
  
  return val;
/* USER CODE END  BSP_PWR_VBUSGetVoltage */
}

/**
  * @brief  Get actual current level measured on the VBUS line.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
 * @retval Current measured current level (in mA)
  */
int32_t BSP_PWR_VBUSGetCurrent(uint32_t PortId)
{
/* USER CODE BEGIN  BSP_PWR_VBUSGetCurrent */
  return 0;
/* USER CODE END  BSP_PWR_VBUSGetCurrent */
}

/**
  * @brief  Initialize VCONN sourcing.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_CC1
  *         @arg TYPE_C_CC2
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VCONNInit(uint32_t PortId,
                                    uint32_t CCPinId)
{
/* USER CODE BEGIN  BSP_PWR_VCONNInit */
  return PWR_ERROR;
/* USER CODE END  BSP_PWR_VCONNInit */
}

/**
  * @brief  Un-Initialize VCONN sourcing.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_CC1
  *         @arg TYPE_C_CC2
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VCONNDeInit(uint32_t PortId,
                                      uint32_t CCPinId)
{
/* USER CODE BEGIN  BSP_PWR_VCONNDeInit */
  return PWR_ERROR;
/* USER CODE END  BSP_PWR_VCONNDeInit */
}

/**
  * @brief  Enable VCONN sourcing.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_CC1
  *         @arg TYPE_C_CC2
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VCONNOn(uint32_t PortId,
                                  uint32_t CCPinId)
{
/* USER CODE BEGIN  BSP_PWR_VCONNOn */
  return PWR_ERROR;
/* USER CODE END  BSP_PWR_VCONNOn */
}

/**
  * @brief  Disable VCONN sourcing.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  CCPinId CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_CC1
  *         @arg TYPE_C_CC2
  * @retval PD controller status
  */
PWR_StatusTypeDef BSP_PWR_VCONNOff(uint32_t PortId,
                                   uint32_t CCPinId)
{
/* USER CODE BEGIN  BSP_PWR_VCONNOff */
  return PWR_ERROR;
/* USER CODE END  BSP_PWR_VCONNOff */
}

/**
  * @brief  Set the VBUS disconnection voltage threshold.
  * @note   Callback funtion registered through  BSP_PWR_RegisterVBUSDetectCallback
  *         function call is invoked when VBUS falls below programmed threshold.
  * @note   By default VBUS disconnection threshold is set to 3.3V
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  VoltageThreshold: VBUS disconnection voltage threshold (in mV)
  * @retval PD controller status
  */
void BSP_PWR_SetVBUSDisconnectionThreshold(uint32_t PortId,
                                           uint32_t VoltageThreshold)
{
/* USER CODE BEGIN  BSP_PWR_SetVBUSDisconnectionThreshold */
/* USER CODE END  BSP_PWR_SetVBUSDisconnectionThreshold */
}

/**
  * @brief  Register USB Type-C Current callback function.
  * @note   Callback function invoked when VBUS rises above 4V (VBUS present) or
  *         when VBUS falls below programmed threshold (VBUS absent).
  * @note   Callback funtion is un-registered when callback function pointer
  *         argument is NULL.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  *         @arg TYPE_C_PORT_2
  * @param  pfnVBUSDetectCallback callback function pointer
  * @retval 0 success else fail
  */
PWR_StatusTypeDef BSP_PWR_RegisterVBUSDetectCallback(uint32_t                       PortId,
                                                     PWR_VBUSDetectCallbackFunc *   pfnVBUSDetectCallback)
{
/* USER CODE BEGIN  BSP_PWR_RegisterVBUSDetectCallback */
  return PWR_ERROR;
/* USER CODE END  BSP_PWR_RegisterVBUSDetectCallback */
}

/**
  * @brief  Get actual VBUS status.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
 * @retval VBUS status (1: On, 0: Off)
  */
uint8_t BSP_PWR_VBUSIsOn(uint32_t PortId)
{
/* USER CODE BEGIN  BSP_PWR_VBUSIsOn */
  uint8_t ret = 0u;

  if (IS_SOURCE_EN_SET_ON())
  {
    ret = 1;
  }

  return ret;
/* USER CODE END  BSP_PWR_VBUSIsOn */
}

/**
  * @brief  Get actual VCONN status.
  * @param  PortId Type-C port identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_PORT_1
  * @param  CCPinId Type-C CC pin identifier
  *         This parameter can be take one of the following values:
  *         @arg TYPE_C_CC1
  *         @arg TYPE_C_CC2
 * @retval VCONN status (1: On, 0: Off)
  */
uint8_t BSP_PWR_VCONNIsOn(uint32_t PortId,
                          uint32_t CCPinId)
{
/* USER CODE BEGIN  BSP_PWR_VCONNIsOn */
  return 0;
/* USER CODE END  BSP_PWR_VCONNIsOn */
}

/**
 * @brief  Get DCDC control mode.
 * @param  PortId Type-C port identifier
 *         This parameter can be take one of the following values:
 *         @arg TYPE_C_PORT_1
 * @param  pDCDCCtrl Pointer on DCDC control mode
 *         Returned value can take one of the following values:
 *         @arg DCDC_CTRL_MODE_UNKNOWN
 *         @arg DCDC_CTRL_MODE_GPIO
 *         @arg DCDC_CTRL_MODE_PWM
  * @retval PD controller status
 */
int32_t  BSP_PWR_DCDCGetCtrlMode(uint32_t PortId, PWR_DCDCCtrlModeTypeDef *pDCDCCtrl)
{
/* USER CODE BEGIN  BSP_PWR_DCDCGetCtrlMode */
  *pDCDCCtrl = DCDC_CTRL_MODE_UNKNOWN;
  return PWR_ERROR;
/* USER CODE END  BSP_PWR_DCDCGetCtrlMode */
}

/**
  * @}
  */


/** @addtogroup STM32G474E_EVAL_USBPD_PWR_Private_Functions
  * @{
  */

/* USER CODE BEGIN POWER_Private_Functions */
/**
 * @brief  Force the VBUS discharge.
 * @param  PortId Type-C port identifier
 *         This parameter can be take one of the following values:
 *         @arg TYPE_C_PORT_1
 * @param  VbusStopInmV Low VBUS threshold (in mV)
 * @param  VbusErrorInmV VBUS margin (in mV)
 * @param  Delay Discharge delay (in ms)
 * @retval None
 */
void PWR_VBUSDischarge(uint32_t PortId,
                           uint32_t VbusStopInmV,
                           uint32_t VbusErrorInmV,
                           uint16_t Delay)
{
/* USER CODE BEGIN BSP_PWR_VBUSDischarge */
  uint32_t vbus;

  DISCHARGE_SET_ON();

  do{
    HAL_Delay(1);
    vbus = BSP_PWR_VBUSGetVoltage(PortId);
  } while(vbus >= (VbusStopInmV));

  /* Need to keep DISCHARGE ON as soon as not powering VBUS on MB1397 */
/* USER CODE END  BSP_PWR_VBUSDischarge */
}

static void PWR_Configure_ADC(void)
{
  /*## Configuration of GPIO used by ADC channels ############################*/
  uint32_t wait_loop_index;

  /* Note: On this STM32 device, ADC1 channel 0 is mapped on GPIO pin PA.00 */

  /* Enable GPIO Clock */
  VSENSE_GPIO_ENABLE_CLOCK();

  /* Configure GPIO in analog mode to be used as ADC input */
  LL_GPIO_SetPinMode(VSENSE_GPIO_PORT, VSENSE_GPIO_PIN, LL_GPIO_MODE_ANALOG);

  /*## Configuration of ADC ##################################################*/

  /*## Configuration of ADC hierarchical scope: common to several ADC ########*/

  /* Enable ADC clock (core clock) */
  VSENSE_ADC_ENABLE_CLOCK();

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       All ADC instances of the ADC common group must be disabled.        */
  /* Note: In this example, all these checks are not necessary but are        */
  /*       implemented anyway to show the best practice usages                */
  /*       corresponding to reference manual procedure.                       */
  /*       Software can be optimized by removing some of these checks, if     */
  /*       they are not relevant considering previous settings and actions    */
  /*       in user application.                                               */
  uint32_t IsEnabled = __LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE(VSENSE_ADC_COMMON);
  if (IsEnabled == 0)
  {
    /* Note: Call of the functions below are commented because they are       */
    /*       useless in this example:                                         */
    /*       setting corresponding to default configuration from reset state. */

    /* Set ADC clock (conversion clock) common to several ADC instances */
    /* Note: On this STM32 serie, ADC common clock asynchonous prescaler      */
    /*       is applied to each ADC instance if ADC instance clock is         */
    /*       set to clock source asynchronous                                 */
    /*       (refer to function "LL_ADC_SetClock()" below).                   */
    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(VSENSE_ADC_INSTANCE), LL_ADC_CLOCK_SYNC_PCLK_DIV4);

    /* Set ADC measurement path to internal channels */
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(VSENSE_ADC_INSTANCE), LL_ADC_PATH_INTERNAL_VREFINT);

    /*## Configuration of ADC hierarchical scope: multimode ####################*/

    /* Note: Feature not available on this STM32 serie */
  }

  /*## Configuration of ADC hierarchical scope: ADC instance #################*/

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       ADC must be disabled.                                              */
  if (LL_ADC_IsEnabled(VSENSE_ADC_INSTANCE) == 0)
  {
    /* Delay for internal voltage reference stabilization time.               */
    /* Compute number of CPU cycles to wait for, from delay in us.            */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    /* Note: If system core clock frequency is below 200kHz, wait time        */
    /*       is only a few CPU processing cycles.                             */
    wait_loop_index = ((LL_ADC_DELAY_VREFINT_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while (wait_loop_index != 0)
    {
      wait_loop_index--;
    }
  }

  /*## Configuration of ADC hierarchical scope: ADC group regular ############*/

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       ADC must be disabled or enabled without conversion on going        */
  /*       on group regular.                                                  */
  if ((LL_ADC_IsEnabled(VSENSE_ADC_INSTANCE) == 0)               ||
      (LL_ADC_REG_IsConversionOngoing(VSENSE_ADC_INSTANCE) == 0))
  {
    /* Set ADC group regular trigger source */
    LL_ADC_REG_SetTriggerSource(VSENSE_ADC_INSTANCE, LL_ADC_REG_TRIG_SOFTWARE);

    /* Set ADC group regular continuous mode */
    LL_ADC_REG_SetContinuousMode(VSENSE_ADC_INSTANCE, LL_ADC_REG_CONV_CONTINUOUS);

    /* Set ADC group regular overrun behavior */
    LL_ADC_REG_SetOverrun(VSENSE_ADC_INSTANCE, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

    /* Set ADC group regular sequencer */
    /* Note: On this STM32 serie, ADC group regular sequencer has             */
    /*       two settings:                                                    */
    /*       - Sequencer configured to fully configurable:                    */
    /*         sequencer length and each rank                                 */
    /*         affectation to a channel are configurable.                     */
    /*         Channels selection is limited to channels 0 to 14.             */
    /*       - Sequencer configured to not fully configurable:                */
    /*         sequencer length and each rank affectation to a channel        */
    /*         are fixed by channel HW number.                                */
    /*         Channels selection is not limited (channels 0 to 18).          */
    /*       Refer to description of function                                 */
    /*       "LL_ADC_REG_SetSequencerConfigurable()".                         */

    /* Set ADC group regular sequence: channel on rank corresponding to       */
    /* channel number.                                                        */
    LL_ADC_REG_SetSequencerRanks(VSENSE_ADC_INSTANCE, VSENSE_ADC_RANK, VSENSE_ADC_CHANNEL);

  }

  /*## Configuration of ADC hierarchical scope: ADC group injected ###########*/

  /* Note: Feature available on this STM32 serie but not used */

  /*## Configuration of ADC hierarchical scope: channels #####################*/

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       ADC must be disabled or enabled without conversion on going        */
  /*       on either groups regular or injected.                              */
  if ((LL_ADC_IsEnabled(VSENSE_ADC_INSTANCE) == 0)                    ||
      ((LL_ADC_REG_IsConversionOngoing(VSENSE_ADC_INSTANCE) == 0) &&
       (LL_ADC_INJ_IsConversionOngoing(VSENSE_ADC_INSTANCE) == 0)))
  {
    /* Set ADC channels sampling time */
    LL_ADC_SetChannelSamplingTime(VSENSE_ADC_INSTANCE, VSENSE_ADC_CHANNEL, LL_ADC_SAMPLINGTIME_247CYCLES_5);
  }

  /*## Configuration of ADC transversal scope: analog watchdog ###############*/

  /* Note: On this STM32 serie, there is only 1 analog watchdog available.    */

  /* Set ADC analog watchdog: channels to be monitored */
  LL_ADC_SetAnalogWDMonitChannels(VSENSE_ADC_INSTANCE, LL_ADC_AWD1, LL_ADC_AWD_ALL_CHANNELS_REG);

  /* Set ADC analog watchdog: thresholds */
  LL_ADC_ConfigAnalogWDThresholds(VSENSE_ADC_INSTANCE, LL_ADC_AWD1, 700, 600);

  /*## Configuration of ADC transversal scope: oversampling ##################*/

  /* Set ADC oversampling scope */
  LL_ADC_SetOverSamplingScope(VSENSE_ADC_INSTANCE, LL_ADC_OVS_GRP_REGULAR_CONTINUED);

  /* Set ADC oversampling parameters */
  LL_ADC_ConfigOverSamplingRatioShift(VSENSE_ADC_INSTANCE, LL_ADC_OVS_RATIO_16, LL_ADC_OVS_SHIFT_RIGHT_4);

  /*## Configuration of ADC interruptions ####################################*/
  /* Enable ADC analog watchdog 1 interruption */
  LL_ADC_EnableIT_AWD1(VSENSE_ADC_INSTANCE);
}

static void PWR_Activate_ADC(void)
{
  __IO uint32_t wait_loop_index = 0;
  __IO uint32_t backup_setting_adc_dma_transfer;
#if (USE_TIMEOUT == 1)
  uint32_t Timeout = 0; /* Variable used for timeout management */
#endif /* USE_TIMEOUT */

  /*## Operation on ADC hierarchical scope: ADC instance #####################*/

  /* Note: Hardware constraint (refer to description of the functions         */
  /*       below):                                                            */
  /*       On this STM32 serie, setting of these features is conditioned to   */
  /*       ADC state:                                                         */
  /*       ADC must be disabled.                                              */
  /* Note: In this example, all these checks are not necessary but are        */
  /*       implemented anyway to show the best practice usages                */
  /*       corresponding to reference manual procedure.                       */
  /*       Software can be optimized by removing some of these checks, if     */
  /*       they are not relevant considering previous settings and actions    */
  /*       in user application.                                               */
  if (LL_ADC_IsEnabled(VSENSE_ADC_INSTANCE) == 0)
  {
    /* Disable ADC deep power down mode */
    LL_ADC_DisableDeepPowerDown(VSENSE_ADC_INSTANCE);

    /* Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(VSENSE_ADC_INSTANCE);

    /* Delay for ADC internal voltage regulator stabilization.                */
    /* Compute number of CPU cycles to wait for, from delay in us.            */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    /* Note: If system core clock frequency is below 200kHz, wait time        */
    /*       is only a few CPU processing cycles.                             */
    wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while (wait_loop_index != 0)
    {
      wait_loop_index--;
    }

    /* Disable ADC DMA transfer request during calibration */
    /* Note: Specificity of this STM32 serie: Calibration factor is           */
    /*       available in data register and also transfered by DMA.           */
    /*       To not insert ADC calibration factor among ADC conversion data   */
    /*       in DMA destination address, DMA transfer must be disabled during */
    /*       calibration.                                                     */
    backup_setting_adc_dma_transfer = LL_ADC_REG_GetDMATransfer(VSENSE_ADC_INSTANCE);
    LL_ADC_REG_SetDMATransfer(VSENSE_ADC_INSTANCE, LL_ADC_REG_DMA_TRANSFER_NONE);

    /* Run ADC self calibration */
    LL_ADC_StartCalibration(VSENSE_ADC_INSTANCE, LL_ADC_SINGLE_ENDED);

    /* Poll for ADC effectively calibrated */
#if (USE_TIMEOUT == 1)
    Timeout = ADC_CALIBRATION_TIMEOUT_MS;
#endif /* USE_TIMEOUT */

    while (LL_ADC_IsCalibrationOnGoing(VSENSE_ADC_INSTANCE) != 0)
    {
#if (USE_TIMEOUT == 1)
      /* Check Systick counter flag to decrement the time-out value */
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        if (Timeout-- == 0)
        {
          /* Time-out occurred. Set LED to blinking mode */
          LED_Blinking(LED_BLINK_ERROR);
        }
      }
#endif /* USE_TIMEOUT */
    }

    /* Restore ADC DMA transfer request after calibration */
    LL_ADC_REG_SetDMATransfer(VSENSE_ADC_INSTANCE, backup_setting_adc_dma_transfer);

    /* Delay between ADC end of calibration and ADC enable.                   */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    wait_loop_index = (ADC_DELAY_CALIB_ENABLE_CPU_CYCLES >> 1);
    while (wait_loop_index != 0)
    {
      wait_loop_index--;
    }

    /* Enable ADC */
    LL_ADC_Enable(VSENSE_ADC_INSTANCE);

    /* Poll for ADC ready to convert */
#if (USE_TIMEOUT == 1)
    Timeout = ADC_ENABLE_TIMEOUT_MS;
#endif /* USE_TIMEOUT */

    while (LL_ADC_IsActiveFlag_ADRDY(VSENSE_ADC_INSTANCE) == 0)
    {
#if (USE_TIMEOUT == 1)
      /* Check Systick counter flag to decrement the time-out value */
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        if (Timeout-- == 0)
        {
          /* Time-out occurred. Set LED to blinking mode */
          LED_Blinking(LED_BLINK_ERROR);
        }
      }
#endif /* USE_TIMEOUT */
    }

    /* Note: ADC flag ADRDY is not cleared here to be able to check ADC       */
    /*       status afterwards.                                               */
    /*       This flag should be cleared at ADC Deactivation, before a new    */
    /*       ADC activation, using function "LL_ADC_ClearFlag_ADRDY()".       */
  }

  /*## Operation on ADC hierarchical scope: ADC group regular ################*/
  /* Note: No operation on ADC group regular performed here.                  */
  /*       ADC group regular conversions to be performed after this function  */
  /*       using function:                                                    */
  /*       "LL_ADC_REG_StartConversion();"                                    */

  /*## Operation on ADC hierarchical scope: ADC group injected ###############*/
  /* Note: Feature not available on this STM32 serie */
}

/* USER CODE END POWER_Private_Functions */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

