/**
  ******************************************************************************
  * @file    stm32g474e_eval.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage
  *          LEDs
  *          Push-buttons
  *          Joystick
  *          POTentiometer
  *          COM ports
  *          available on STM32G474E-EVAL board (MB1397) from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval.h"

#if (USE_BSP_COM_FEATURE > 0)
  #if (USE_COM_LOG > 0)
    #ifndef __GNUC__
      #include <stdio.h>
    #endif
  #endif
#endif

/** @addtogroup BSP
  * @{
  */

/** @defgroup STM32G474E-EVAL STM32G474E-EVAL
  * @{
  */

/** @defgroup STM32G474E-EVAL_Common STM32G474E-EVAL Common
  * @brief This file provides set of firmware functions to manage Leds and push-button
  *        available on STM32G474E-EVAL Kit from STMicroelectronics.
  * @{
  */

/** @defgroup STM32G474E-EVAL_Private_TypesDefinitions Private Types Definitions 
  * @{
  */
typedef void (* BSP_EXTI_LineCallback) (void);
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_Private_Variables Private Variables
  * @{
  */
static GPIO_TypeDef* LED_PORT[LEDn] = {LED1_GPIO_PORT,
                                       0,
                                       LED3_GPIO_PORT,
                                       0,
                                       LED_BUCK_GPIO_PORT};
#if (USE_BSP_IO_CLASS > 0)
static const uint32_t LED_PIN[LEDn] = {LED1_PIN,
                                       LED2_PIN,
                                       LED3_PIN,
                                       LED4_PIN,
                                       LED_BUCK_PIN};
#else
static const uint32_t LED_PIN[LEDn] = {LED1_PIN,
                                       0,
                                       LED3_PIN,
                                       0,
                                       LED_BUCK_PIN};
#endif /* (USE_BSP_IO_CLASS > 0) */
static GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {USER_BUTTON_GPIO_PORT};
static const uint16_t BUTTON_PIN[BUTTONn] = {USER_BUTTON_PIN};
static const IRQn_Type BUTTON_IRQn[BUTTONn] = {USER_BUTTON_EXTI_IRQn};

#if (USE_BSP_COM_FEATURE > 0)
static USART_TypeDef* COM_USART[COMn]   = {COM1_UART};
#if (USE_COM_LOG > 0)
static COM_TypeDef COM_ActiveLogPort = COM1;
#endif
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
static uint32_t IsComMspCbValid[COMn] = {0};
#endif
#endif /* (USE_BSP_COM_FEATURE > 0)*/

#if (USE_BSP_POT_FEATURE > 0)
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1) || (USE_HAL_COMP_REGISTER_CALLBACKS == 1) || (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
static uint32_t IsPotMspCbValid[POTn] = {0};
#endif  /* USE_HAL_XXX_REGISTER_CALLBACKS */
#endif  /* (USE_BSP_POT_FEATURE > 0) */

#if (USE_BSP_IO_CLASS > 0)
#if (USE_BSP_JOY_FEATURE > 0)
static uint16_t CurrentJoyPins[JOYn]  = {0};
static uint32_t JoyPinsMask[JOYn];

static const uint16_t JOY_NONE_PIN[JOYn]  = {JOY_NONE};
static const uint16_t JOY_SEL_PIN[JOYn]   = {JOY_SEL};
static const uint16_t JOY_DOWN_PIN[JOYn]  = {JOY_DOWN};
static const uint16_t JOY_LEFT_PIN[JOYn]  = {JOY_LEFT};
static const uint16_t JOY_RIGHT_PIN[JOYn] = {JOY_RIGHT};
static const uint16_t JOY_UP_PIN[JOYn]    = {JOY_UP};
static const uint16_t JOY_ALL_PIN[JOYn]   = {JOY_ALL};
#endif /* USE_BSP_JOY_FEATURE */
#endif /* USE_BSP_IO_CLASS */
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_Exported_Variables Exported Variables
  * @{
  */

#if (USE_BSP_IO_CLASS > 0)
#if (USE_BSP_JOY_FEATURE > 0)
EXTI_HandleTypeDef hjoy_exti[JOYn];
#endif /* USE_BSP_JOY_FEATURE */
#endif /* USE_BSP_IO_CLASS */
#if (USE_BSP_COM_FEATURE > 0)
UART_HandleTypeDef hcom_uart[COMn];
#endif /* USE_BSP_COM_FEATURE */
EXTI_HandleTypeDef hpb_exti[BUTTONn];

#if (USE_BSP_POT_FEATURE > 0)
OPAMP_HandleTypeDef hpot_opamp[POTn];
ADC_HandleTypeDef hpot_adc[POTn];
#if (USE_BSP_POT_COMP_FEATURE > 0)
COMP_HandleTypeDef hpot_comp[POTn];
#endif /* (USE_BSP_POT_COMP_FEATURE > 0)*/
#endif  /* (USE_BSP_POT_FEATURE > 0) */
/**
  * @}
  */

/** STM32G474E-EVAL_Private_FunctionPrototypes Private Functions Prototypes
  */

#if (USE_BSP_POT_FEATURE > 0)
static void OPAMP4_MspInit(OPAMP_HandleTypeDef *hopamp);
static void OPAMP4_MspDeInit(OPAMP_HandleTypeDef *hopamp);
static void ADC5_MspInit(ADC_HandleTypeDef *hadc);
static void ADC5_MspDeInit(ADC_HandleTypeDef *hadc);
#if (USE_BSP_POT_COMP_FEATURE > 0)
static void COMP6_MspInit(COMP_HandleTypeDef *hcomp);
static void COMP6_MspDeInit(COMP_HandleTypeDef *hcomp);
#endif  /* (USE_BSP_POT_COMP_FEATURE > 0) */
#endif  /* (USE_BSP_POT_FEATURE > 0) */

#if (USE_BSP_COM_FEATURE > 0)
static void USART1_MspInit(UART_HandleTypeDef *huart);
static void USART1_MspDeInit(UART_HandleTypeDef *huart);
#endif

#if (USE_BSP_IO_CLASS > 0)
#if (USE_BSP_JOY_FEATURE > 0)
static void JOY1_EXTI_Callback(void);
#endif /* USE_BSP_JOY_FEATURE */
#endif /* USE_BSP_IO_CLASS */

static void BUTTON_USER_EXTI_Callback(void);
/**
  */

/** @defgroup STM32G474E-EVAL_Exported_Functions Exported Functions
  * @{
  */

  /**
  * @brief  This method returns the STM32G474E-EVAL BSP Driver revision
  * @retval version: 0xXYZR (8bits for each decimal, R for RC)
  */
int32_t BSP_GetVersion(void)
{
  return (int32_t)STM32G474E_EVAL_BSP_VERSION;
}

#if defined(_GUI_INTERFACE)
/**
  * @brief  This method returns HW board version name
  * @retval HW Board version name
  */
const uint8_t* BSP_GetHWBoardVersionName(void)
{
  return HWBoardVersionName;
}

/**
  * @brief  This method returns HW PD Type name
  * @retval HW Board version name
  */
const uint8_t* BSP_GetPDTypeName(void)
{
  return PDTypeName;
}

#endif /* _GUI_INTERFACE */
/**
  * @brief  Configures LED on GPIO.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  *            @arg  LED_BUCK
  * @retval BSP status
  */
int32_t BSP_LED_Init(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;

#if (USE_BSP_IO_CLASS > 0)
  BSP_IO_Init_t io_init_structure;
#endif /* USE_BSP_IO_CLASS */
  GPIO_InitTypeDef  gpio_init_structure;

  if((Led == LED1) || (Led == LED3) || (Led == LED_BUCK))
  {
    /* Enable the GPIO_LED clock */
    if(Led == LED1)
    {
      LED1_GPIO_CLK_ENABLE();
    }
    else if(Led == LED3)
    {
      LED3_GPIO_CLK_ENABLE();
    }
    else
    {
      LED_BUCK_GPIO_CLK_ENABLE();
    }

   /* Configure the GPIO_LED pin */
   gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
   gpio_init_structure.Pull = GPIO_PULLUP;
   gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
   gpio_init_structure.Pin = LED_PIN [Led];
   HAL_GPIO_Init(LED_PORT [Led], &gpio_init_structure);
   HAL_GPIO_WritePin(LED_PORT [Led], (uint16_t)LED_PIN[Led], GPIO_PIN_SET);
  }
  else
  {
#if (USE_BSP_IO_CLASS > 0)
   io_init_structure.Pin  = LED_PIN[Led];
   io_init_structure.Pull = IO_NOPULL;
   io_init_structure.Mode = IO_MODE_OUTPUT_PP;
   /* Initialize MFX */
   if(BSP_IO_Init(0, &io_init_structure) != BSP_ERROR_NONE)
   {
    ret = BSP_ERROR_NO_INIT;
   }
   else
   {
     if(BSP_IO_WritePin(0, (uint16_t)LED_PIN[Led], IO_PIN_SET) != BSP_ERROR_NONE)
     {
      ret = BSP_ERROR_NO_INIT;
     }
   }
#endif /* USE_BSP_IO_CLASS */
  }

  return ret;
}

/**
  * @brief  DeInit LEDs.
  * @param  Led LED to be configured.
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  *            @arg  LED_BUCK
  * @retval BSP status
  */
int32_t BSP_LED_DeInit(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
#if (USE_BSP_IO_CLASS > 0)
  BSP_IO_Init_t io_init_structure;
#endif /* USE_BSP_IO_CLASS */
  GPIO_InitTypeDef  gpio_init_structure;

  if((Led == LED1) || (Led == LED3) || (Led == LED_BUCK))
  {
   /* Configure the GPIO_LED pin */
   gpio_init_structure.Pin = LED_PIN [Led];
   HAL_GPIO_WritePin(LED_PORT [Led], (uint16_t)LED_PIN[Led], GPIO_PIN_SET);

   HAL_GPIO_DeInit(LED_PORT [Led], gpio_init_structure.Pin);
  }
  else
  {
#if (USE_BSP_IO_CLASS > 0)
   if(BSP_IO_WritePin(0, (uint16_t)LED_PIN[Led], IO_PIN_RESET) != BSP_ERROR_NONE)
   {
     ret = BSP_ERROR_NO_INIT;
   }
   else
   {
    io_init_structure.Pin  = LED_PIN[Led];
    io_init_structure.Pull = IO_PULLUP;
    io_init_structure.Mode = IO_MODE_OFF;

    if(BSP_IO_Init(0, &io_init_structure) != BSP_ERROR_NONE)
    {
     ret = BSP_ERROR_NO_INIT;
    }
   }
#endif /* USE_BSP_IO_CLASS */
  }
  return ret;
}

/**
  * @brief  Turns selected LED On.
  * @param  Led LED to be set on
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  *            @arg  LED_BUCK
  * @retval BSP status
  */
int32_t BSP_LED_On(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;

  if((Led == LED1) || (Led == LED3))
  {
    HAL_GPIO_WritePin (LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_PIN_RESET);
  }
  else if(Led == LED_BUCK)
  {
    HAL_GPIO_WritePin (LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_PIN_SET);
  }
  else
  {
#if (USE_BSP_IO_CLASS > 0)
   ret = BSP_IO_WritePin(0, (uint16_t)LED_PIN[Led], IO_PIN_RESET);
#endif /* USE_BSP_IO_CLASS */
  }
  return ret;
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led LED to be set off
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  *            @arg  LED_BUCK
  * @retval BSP status
  */
int32_t BSP_LED_Off(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  if((Led == LED1) || (Led == LED3))
  {
   HAL_GPIO_WritePin (LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_PIN_SET);
  }
  else if(Led == LED_BUCK)
  {
    HAL_GPIO_WritePin (LED_PORT [Led], (uint16_t)LED_PIN [Led], GPIO_PIN_RESET);
  }
  else
  {
#if (USE_BSP_IO_CLASS > 0)
   ret = BSP_IO_WritePin(0, LED_PIN[Led], IO_PIN_SET);
#endif /* USE_BSP_IO_CLASS */
  }
  return ret;
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led LED to be toggled
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  *            @arg  LED_BUCK
  * @retval BSP status
  */
int32_t BSP_LED_Toggle(Led_TypeDef Led)
{
  int32_t ret = BSP_ERROR_NONE;
  if((Led == LED1) || (Led == LED3) || (Led == LED_BUCK))
  {
   HAL_GPIO_TogglePin(LED_PORT[Led], (uint16_t)LED_PIN[Led]);
  }
  else
  {
#if (USE_BSP_IO_CLASS > 0 )
   ret = BSP_IO_TogglePin(0, LED_PIN[Led]);
#endif /* USE_BSP_IO_CLASS */
  }

  return ret;
}

/**
  * @brief  Get the selected LED state.
  * @param  Led LED to be get its state
  *          This parameter can be one of the following values:
  *            @arg  LED1
  *            @arg  LED2
  *            @arg  LED3
  *            @arg  LED4
  *            @arg  LED_BUCK
  * @retval LED status
  */
int32_t BSP_LED_GetState (Led_TypeDef Led)
{
  int32_t ret;

  if((Led == LED1) || (Led == LED3))
  {
   ret = (int32_t)((HAL_GPIO_ReadPin (LED_PORT [Led], (uint16_t)LED_PIN [Led]) != GPIO_PIN_RESET) ? 0U : 1U);
  }
  else if(Led == LED_BUCK)
  {
   ret = (int32_t)((HAL_GPIO_ReadPin (LED_PORT [Led], (uint16_t)LED_PIN [Led]) != GPIO_PIN_RESET) ? 0U : 1U);
  }
  else
  {
#if (USE_BSP_IO_CLASS > 0 )
    ret = (int32_t)(((BSP_IO_ReadPin(0, (uint32_t)LED_PIN[Led])) != (int32_t)IO_PIN_RESET) ? 0U : 1U);
#else
    ret = BSP_ERROR_WRONG_PARAM;
#endif /* USE_BSP_IO_CLASS */
  }
  return ret;
}

/**
  * @brief  Configures button GPIO and EXTI Line.
  * @param  Button: Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_USER: User Push Button
  * @param  ButtonMode Button mode
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
  *            @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line
  *                                    with interrupt generation capability
  * @retval BSP status
  */
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef gpio_init_structure;
  static BSP_EXTI_LineCallback ButtonCallback[BUTTONn] ={BUTTON_USER_EXTI_Callback};
  static uint32_t  BSP_BUTTON_PRIO [BUTTONn] ={BSP_BUTTON_USER_IT_PRIORITY};
  static const uint32_t BUTTON_EXTI_LINE[BUTTONn] ={USER_BUTTON_EXTI_LINE};

  /* Enable the BUTTON clock*/
  USER_BUTTON_GPIO_CLK_ENABLE();
  gpio_init_structure.Pin = BUTTON_PIN [Button];
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

  if(ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(BUTTON_PORT [Button], &gpio_init_structure);
  }
  else /* (ButtonMode == BUTTON_MODE_EXTI) */
  {
    /* Configure Button pin as input with External interrupt */
    gpio_init_structure.Mode = GPIO_MODE_IT_FALLING;

    HAL_GPIO_Init(BUTTON_PORT[Button], &gpio_init_structure);

    (void)HAL_EXTI_GetHandle(&hpb_exti[Button], BUTTON_EXTI_LINE[Button]);
    (void)HAL_EXTI_RegisterCallback(&hpb_exti[Button],  HAL_EXTI_COMMON_CB_ID, ButtonCallback[Button]);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((BUTTON_IRQn[Button]), BSP_BUTTON_PRIO[Button], 0x00);
    HAL_NVIC_EnableIRQ((BUTTON_IRQn[Button]));
  }
  return BSP_ERROR_NONE;
}

/**
  * @brief  Push Button DeInit.
  * @param  Button Button to be configured
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_USER User Push Button
  * @note PB DeInit does not disable the GPIO clock
  * @retval BSP status
  */

int32_t BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef gpio_init_structure;

  gpio_init_structure.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
  HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);

  return BSP_ERROR_NONE;
}

/**
  * @brief  Returns the selected button state.
  * @param  Button Button to be checked
  *          This parameter can be one of the following values:
  *            @arg  BUTTON_USER User Push Button
  * @retval The Button GPIO pin value
  */
int32_t BSP_PB_GetState(Button_TypeDef Button)
{
  return (int32_t)HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/**
  * @brief  This function handles EXTI_LINE_15 interrupt request.
  * @retval None
  */
void BSP_PB_IRQHandler(Button_TypeDef Button)
{
  HAL_EXTI_IRQHandler(&hpb_exti[Button]);
}

/**
  * @brief  BSP Push Button callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None.
  */
__weak void BSP_PB_Callback(Button_TypeDef Button)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Button);
  /* This function should be implemented by the user application.
     It is called into this driver when an event on Button is triggered. */
}

/**
  * @brief  Key EXTI line detection callbacks.
  * @retval None
  */
static void BUTTON_USER_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_USER);
}

#if (USE_BSP_COM_FEATURE > 0)
/**
  * @brief  Configures COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be COM1
  * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
  *                configuration information for the specified USART peripheral.
  * @retval BSP status
  */
int32_t BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM > COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    hcom_uart[COM].Instance          = COM_USART[COM];

    /* Init the UART Msp */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    USART1_MspInit(&hcom_uart[COM]);
#else
    if(IsComMspCbValid[COM] == 0U)
    {
      if(BSP_COM_RegisterDefaultMspCallbacks(COM) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif

    if(MX_USART1_Init(&hcom_uart[COM], COM_Init) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  DeInit COM port.
  * @param  COM COM port to be configured.
  *          This parameter can be COM1
  * @retval BSP status
  */
int32_t BSP_COM_DeInit(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM > COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* USART configuration */
    hcom_uart[COM].Instance = COM_USART[COM];

#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
    USART1_MspDeInit(&hcom_uart[COM]);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 0) */

    if(HAL_UART_DeInit(&hcom_uart[COM]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  Configures COM port.
  * @param  huart USART handle
  * @param  Init Pointer to a UART_HandleTypeDef structure that contains the
  *                  configuration information for the specified USART peripheral.
  * @retval HAL error code
  */
__weak HAL_StatusTypeDef MX_USART1_Init(UART_HandleTypeDef *huart, MX_UART_InitTypeDef *Init)
{
  /* USART configuration */
  huart->Init.BaudRate     = Init->BaudRate;
  huart->Init.Mode         = UART_MODE_TX_RX;
  huart->Init.Parity       = (uint32_t)Init->Parity;
  huart->Init.WordLength   = Init->WordLength;
  huart->Init.StopBits     = (uint32_t)Init->StopBits;
  huart->Init.HwFlowCtl    = (uint32_t)Init->HwFlowCtl;
  huart->Init.OverSampling = UART_OVERSAMPLING_8;

  return HAL_UART_Init(huart);
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
/**
  * @brief Register Default COM Msp Callbacks
  * @param  COM COM port to be configured.
  * @retval BSP status
  */
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM]);

    /* Register default MspInit/MspDeInit Callback */
    if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, USART1_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, USART1_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }

  /* BSP status */
  return ret;
}

/**
  * @brief  Register COM1 Msp Callback registering
  * @param  COM COM port to be configured.
  * @param  Callbacks     pointer to COM MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;

  if(COM >= COMn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM]);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPINIT_CB_ID, Callback->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_UART_RegisterCallback(&hcom_uart[COM], HAL_UART_MSPDEINIT_CB_ID, Callback->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsComMspCbValid[COM] = 1U;
    }
  }

  /* BSP status */
  return ret;
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

#if (USE_COM_LOG > 0)
/**
  * @brief  Select the active COM port.
  * @param  COM COM port to be activated.
  *          This parameter can be COM1
  * @retval BSP status
  */
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM)
{
  if(COM_ActiveLogPort != COM)
  {
    COM_ActiveLogPort = COM;
  }
  return BSP_ERROR_NONE;
}

/**
  * @brief  Redirect console output to COM
  */
 #ifdef __GNUC__
 int __io_putchar (int ch)
{
 #else
 int fputc (int ch, FILE *f)
{
 UNUSED(f);
 #endif /* __GNUC__ */

  (void)HAL_UART_Transmit (&hcom_uart [COM_ActiveLogPort], (uint8_t *) &ch, 1, COM_POLL_TIMEOUT);
  return ch;
}
#endif /* USE_COM_LOG */
#endif /* USE_BSP_COM_FEATURE*/

#if (USE_BSP_POT_FEATURE > 0)
/**
  * @brief  Init Potentiometer.
  * @param  POT  Potentiometer to be activated.
  * @retval BSP status
  */
int32_t BSP_POT_Init(POT_TypeDef POT)
{
  int32_t ret;

  if(POT >= POTn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
#if (USE_BSP_POT_COMP_FEATURE > 0)
    hpot_comp[POT].Instance           = COMP6;
#endif /* (USE_BSP_POT_COMP_FEATURE > 0) */
    hpot_opamp[POT].Instance          = OPAMP4;
    hpot_adc[POT].Instance            = ADC5;
#if (USE_HAL_OPAMP_REGISTER_CALLBACKS == 0)
    OPAMP4_MspInit(&hpot_opamp[POT]);
#endif /* (USE_HAL_OPAMP_REGISTER_CALLBACKS == 0)*/
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 0)
    /* Init the ADC Msp */
    ADC5_MspInit(&hpot_adc[POT]);
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 0)*/
#if (USE_BSP_POT_COMP_FEATURE > 0)
#if (USE_HAL_COMP_REGISTER_CALLBACKS == 0)
    COMP6_MspInit(&hpot_comp[POT]);
#endif /* (USE_HAL_COMP_REGISTER_CALLBACKS == 0)*/
#endif /* (USE_BSP_POT_COMP_FEATURE > 0) */

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1) || (USE_HAL_COMP_REGISTER_CALLBACKS == 1) || (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
    if(IsPotMspCbValid[POT] == 0U)
    {
      if(BSP_POT_RegisterDefaultMspCallbacks(POT) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_MSP_FAILURE;
      }
    }
#endif /* USE_HAL_XXX_REGISTER_CALLBACKS */
    if(MX_OPAMP4_Init(&hpot_opamp[POT]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(MX_ADC5_Init(&hpot_adc[POT]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
#if (USE_BSP_POT_COMP_FEATURE > 0)
    else if(MX_COMP6_Init(&hpot_comp[POT]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
#endif /* (USE_BSP_POT_COMP_FEATURE > 0) */
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  return ret;
}

/**
  * @brief  De-Initialize the Potentiometer.
  * @param  POT  Potentiometer to be activated.
  * @retval BSP status
  */
int32_t BSP_POT_DeInit(POT_TypeDef POT)
{
  int32_t ret;

  if(POT >= POTn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
#if (USE_HAL_OPAMP_REGISTER_CALLBACKS == 0)
    OPAMP4_MspDeInit(&hpot_opamp[POT]);
#endif /* (USE_HAL_OPAMP_REGISTER_CALLBACKS == 0) */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 0)
    ADC5_MspDeInit(&hpot_adc[POT]);
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS == 0) */
#if (USE_BSP_POT_COMP_FEATURE > 0)
#if (USE_HAL_COMP_REGISTER_CALLBACKS == 0)
    COMP6_MspDeInit(&hpot_comp[POT]);
#endif /* (USE_HAL_COMP_REGISTER_CALLBACKS == 0) */
#endif /* (USE_BSP_POT_COMP_FEATURE > 0) */
    if(MX_OPAMP4_DeInit(&hpot_opamp[POT]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_ADC_DeInit(&hpot_adc[POT]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
#if (USE_BSP_POT_COMP_FEATURE > 0)
    else if(MX_COMP6_DeInit(&hpot_comp[POT]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
#endif /* (USE_BSP_POT_COMP_FEATURE > 0) */
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  return ret;
}

/**
  * @brief  Get Potentiometer level in 12 bits.
  * @retval Potentiometer level(0..100%) / 0xFFFFFFFF : Error
  */
int32_t BSP_POT_GetLevel(POT_TypeDef POT)
{
  int32_t ret = (int32_t)0xFFFFFFFFU;
  uint32_t AdcValue;

  if(POT >= POTn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (HAL_OPAMP_Start(&hpot_opamp[POT]) != HAL_OK)
  {
    ret =  BSP_ERROR_PERIPH_FAILURE;
  }
#if (USE_BSP_POT_COMP_FEATURE > 0)
  else if(HAL_COMP_Start(&hpot_comp[POT]) != HAL_OK)
  {
    ret =  BSP_ERROR_PERIPH_FAILURE;
  }
#endif /* (USE_BSP_POT_COMP_FEATURE > 0) */
  else if(HAL_ADC_Start(&hpot_adc[POT]) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    if(HAL_ADC_PollForConversion(&hpot_adc[POT], POT_ADC_POLL_TIMEOUT) == HAL_OK)
    {
      AdcValue = HAL_ADC_GetValue(&hpot_adc[POT]);
      if(AdcValue <= 0x0FFFU)
      {
        ret = POT_CONVERT2PERC(AdcValue);
      }
    }
    else
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }
  return ret;
}

#if (USE_BSP_POT_COMP_FEATURE > 0)
/**
  * @brief  Configures Comp6 peripheral
  * @param  comp COMP handle
  * @retval HAL error code
  */
__weak HAL_StatusTypeDef MX_COMP6_Init(COMP_HandleTypeDef *hcomp)
{
  HAL_StatusTypeDef ret = HAL_OK;

  /* COMP configuration */
  hcomp->Init.InputPlus      = COMP_INPUT_PLUS_IO1;
  hcomp->Init.InputMinus     = COMP_INPUT_MINUS_1_2VREFINT; /* compare to half Vref */
  hcomp->Init.OutputPol      = COMP_OUTPUTPOL_NONINVERTED;
  hcomp->Init.BlankingSrce   = COMP_BLANKINGSRC_NONE;
  hcomp->Init.TriggerMode    = COMP_TRIGGERMODE_NONE;
  hcomp->Init.DeglitcherMode = COMP_DEGLITCHER_DISABLED;

  if (HAL_COMP_Init(hcomp) != HAL_OK)
  {
    ret =  HAL_ERROR;
  }

  return ret;
}

/**
  * @brief  DeConfigures Comp6 peripheral
  * @param  comp COMP handle
  * @retval HAL error code
  */
__weak HAL_StatusTypeDef MX_COMP6_DeInit(COMP_HandleTypeDef *comp)
{
  HAL_StatusTypeDef ret = HAL_OK;

  if (HAL_COMP_Stop(comp) != HAL_OK)
  {
    ret =  HAL_ERROR;
  }
  else
  {
    if (HAL_COMP_DeInit(comp) != HAL_OK)
    {
      ret =  HAL_ERROR;
    }
  }

  return ret;
}
#endif /* USE_BSP_POT_COMP_FEATURE > 0)*/

/**
  * @brief  Configures OpAmp4 peripheral
  * @param  opamp OPAMP handle
  * @retval HAL error code
  */
__weak HAL_StatusTypeDef MX_OPAMP4_Init(OPAMP_HandleTypeDef *opamp)
{
  HAL_StatusTypeDef ret = HAL_OK;

  /* COMP configuration */
  opamp->Init.InternalOutput    = ENABLE;
  opamp->Init.Mode              = OPAMP_FOLLOWER_MODE;
  opamp->Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO2;

  if (HAL_OPAMP_Init(opamp) != HAL_OK)
  {
    ret =  HAL_ERROR;
  }

  return ret;
}

/**
  * @brief  DeConfigures OpAmp4 peripheral
  * @param  opamp OPAMP handle
  * @retval HAL error code
  */
__weak HAL_StatusTypeDef MX_OPAMP4_DeInit(OPAMP_HandleTypeDef *opamp)
{
  HAL_StatusTypeDef ret = HAL_OK;

  if (HAL_OPAMP_Stop(opamp) != HAL_OK)
  {
    ret =  HAL_ERROR;
  }
  else
  {
    if (HAL_OPAMP_DeInit(opamp) != HAL_OK)
    {
      ret =  HAL_ERROR;
    }
  }

  return ret;
}

/**
  * @brief  Configures ADC5 peripheral
  * @param  hadc ADC handle
  * @retval HAL error code
  */
__weak HAL_StatusTypeDef MX_ADC5_Init(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef ret = HAL_OK;
  ADC_ChannelConfTypeDef sConfig = {0};

  /* ADC configuration */
  hadc->Init.ClockPrescaler        = POT1_PRESCALER;
  hadc->Init.Resolution            = ADC_RESOLUTION_12B;
  hadc->Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc->Init.ScanConvMode          = (uint32_t)DISABLE;
#if defined(ADC_EOC_SINGLE_CONV)
  hadc->Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
#endif
  hadc->Init.ContinuousConvMode    = DISABLE;
  hadc->Init.NbrOfConversion       = 1;
  hadc->Init.DiscontinuousConvMode = DISABLE;
  hadc->Init.NbrOfDiscConversion   = 0;
  hadc->Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc->Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc->Init.DMAContinuousRequests = DISABLE;

  if (HAL_ADC_Init(hadc) != HAL_OK)
  {
    ret =  HAL_ERROR;
  }
  else
  {
    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    */
    sConfig.Channel      = POT1_ADC_CHANNEL;
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = POT1_SAMPLING_TIME;
    sConfig.Offset       = 0;
#if defined(ADC_SINGLE_ENDED)
    sConfig.SingleDiff   = ADC_SINGLE_ENDED;
#endif
#if defined(ADC_OFFSET_NONE)
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
#endif
    if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
    {
      ret =  HAL_ERROR;
    }
  }

  return ret;
}

/**
  * @brief  DeConfigures ADC5 peripheral
  * @param  hadc ADC handle
  * @retval HAL error code
  */
__weak HAL_StatusTypeDef MX_ADC5_DeInit(ADC_HandleTypeDef *hadc)
{
  HAL_StatusTypeDef ret = HAL_OK;

  if (HAL_ADC_Stop(hadc) != HAL_OK)
  {
    ret =  HAL_ERROR;
  }
  else
  {
    if (HAL_ADC_DeInit(hadc) != HAL_OK)
    {
      ret =  HAL_ERROR;
    }
  }

  return ret;
}

#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1) || (USE_HAL_COMP_REGISTER_CALLBACKS == 1)  || (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
/**
  * @brief Register Default Potentiometer Msp Callbacks
  * @param POT Potentiometer instance
  * @retval BSP status
  */
int32_t BSP_POT_RegisterDefaultMspCallbacks (POT_TypeDef POT)
{
  int32_t ret = BSP_ERROR_NONE;

  if(POT >= POTn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
#if (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
    /* Register default MspInit/MspDeInit Callback */
    if(HAL_OPAMP_RegisterCallback(&hpot_opamp[POT], HAL_OPAMP_MSP_INIT_CB_ID, OPAMP4_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_OPAMP_RegisterCallback(&hpot_opamp[POT], HAL_OPAMP_MSP_DEINIT_CB_ID, OPAMP4_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsPotMspCbValid[POT] = 1U;
    }
#endif /*(USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)*/
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
    __HAL_ADC_RESET_HANDLE_STATE(&hpot_adc[POT]);

    /* Register default MspInit/MspDeInit Callback */
    if(HAL_ADC_RegisterCallback(&hpot_adc[POT], HAL_ADC_MSPINIT_CB_ID, ADC5_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_ADC_RegisterCallback(&hpot_adc[POT], HAL_ADC_MSPDEINIT_CB_ID, ADC5_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsPotMspCbValid[POT] = 1U;
    }
#endif /*(USE_HAL_ADC_REGISTER_CALLBACKS == 1)*/
#if (USE_BSP_POT_COMP_FEATURE > 0)
#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
    /* Register default MspInit/MspDeInit Callback */
    if(HAL_COMP_RegisterCallback(&hpot_comp[POT], HAL_COMP_MSPINIT_CB_ID, COMP6_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_COMP_RegisterCallback(&hpot_comp[POT], HAL_COMP_MSPDEINIT_CB_ID, COMP6_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsPotMspCbValid[POT] = 1U;
    }
#endif /*(USE_HAL_COMP_REGISTER_CALLBACKS == 1)*/
#endif /* (USE_BSP_POT_COMP_FEATURE > 0) */
  }

  /* BSP status */
  return ret;
}

/**
  * @brief Register Potentiometer Msp Callback registering
  * @param POT Potentiometer instance
  * @param Callbacks     pointer to Potentiometer MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_POT_RegisterMspCallbacks (POT_TypeDef POT, BSP_POT_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;

  if(POT >= POTn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
#if (USE_HAL_OPAMP_REGISTER_CALLBACKS == 1)
    __HAL_OPAMP_RESET_HANDLE_STATE(&hpot_opamp [POT]);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_OPAMP_RegisterCallback(&hpot_opamp[POT], HAL_OPAMP_MSP_INIT_CB_ID, Callback->pMspOPAMPInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_OPAMP_RegisterCallback(&hpot_opamp[POT], HAL_OPAMP_MSP_DEINIT_CB_ID, Callback->pMspOPAMPDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsPotMspCbValid[POT] = 1U;
    }
#endif /* (USE_HAL_OPAMP_REGISTER_CALLBACKS */
#if (USE_HAL_ADC_REGISTER_CALLBACKS == 1)
    __HAL_ADC_RESET_HANDLE_STATE(&hpot_adc[POT]);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_ADC_RegisterCallback(&hpot_adc[POT], HAL_ADC_MSPINIT_CB_ID, Callback->pMspADCInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_ADC_RegisterCallback(&hpot_adc[POT], HAL_ADC_MSPDEINIT_CB_ID, Callback->pMspADCDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsPotMspCbValid[POT] = 1U;
    }
#endif /* (USE_HAL_ADC_REGISTER_CALLBACKS */
#if (USE_BSP_POT_COMP_FEATURE > 0)
#if (USE_HAL_COMP_REGISTER_CALLBACKS == 1)
    __HAL_COMP_RESET_HANDLE_STATE(&hpot_comp [POT]);

    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_COMP_RegisterCallback(&hpot_comp[POT], HAL_COMP_MSPINIT_CB_ID, Callback->pMspCOMPInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_COMP_RegisterCallback(&hpot_comp[POT], HAL_COMP_MSPDEINIT_CB_ID, Callback->pMspCOMPDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsPotMspCbValid[POT] = 1U;
    }
#endif /* (USE_HAL_COMP_REGISTER_CALLBACKS */
#endif /* (USE_BSP_POT_COMP_FEATURE > 0) */
  }
  /* BSP status */
  return ret;
}
#endif /* USE_HAL_ADC_REGISTER_CALLBACKS */
#endif /*(USE_BSP_POT_FEATURE > 0)*/

#if (USE_BSP_IO_CLASS > 0 )
#if (USE_BSP_JOY_FEATURE > 0)
/**
  * @brief  Configures joystick GPIO and EXTI modes.
  * @param  JOY Joystick to be initialized
  * @param  JoyMode Button mode.
  *          This parameter can be one of the following values:
  *            @arg  JOY_MODE_GPIO: Joystick pins will be used as simple IOs
  *            @arg  JOY_MODE_EXTI: Joystick pins will be connected to EXTI line
  *                                 with interrupt generation capability
  * @param  JoyPins joystick pins to be initialized
  * @retval BSP status
  */
int32_t BSP_JOY_Init(JOY_TypeDef JOY, JOYMode_TypeDef JoyMode, JOYPin_TypeDef JoyPins)
{
  int32_t          ret;
  BSP_IO_Init_t    io_init_structure;
  static BSP_EXTI_LineCallback JoyCallback[JOYn] = {JOY1_EXTI_Callback};
  static const uint32_t JOY_EXTI_LINE[JOYn]   = {JOY1_EXTI_LINE};

  if(JOY >= JOYn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
         /* Initialize the BSP IO driver and configure the joystick pin */
         switch (JoyPins)
         {
           case JOY_SEL:
             io_init_structure.Pin = JOY_SEL_PIN[JOY];
             CurrentJoyPins[JOY] |= JOY_SEL_PIN[JOY];
             break;
           case JOY_DOWN:
             io_init_structure.Pin = JOY_DOWN_PIN[JOY];
             CurrentJoyPins[JOY] |= JOY_DOWN_PIN[JOY];
             break;
           case JOY_LEFT:
             io_init_structure.Pin = JOY_LEFT_PIN[JOY];
             CurrentJoyPins[JOY] |= JOY_LEFT_PIN[JOY];
             break;
           case JOY_RIGHT:
             io_init_structure.Pin = JOY_RIGHT_PIN[JOY];
             CurrentJoyPins[JOY] |= JOY_RIGHT_PIN[JOY];
             break;
           case JOY_UP:
             io_init_structure.Pin = JOY_UP_PIN[JOY];
             CurrentJoyPins[JOY] |= JOY_UP_PIN[JOY];
             break;
           case JOY_ALL:
             io_init_structure.Pin = JOY_ALL_PIN[JOY];
             CurrentJoyPins[JOY] |= JOY_ALL_PIN[JOY];
             break;
           default :
             CurrentJoyPins[JOY] |= JOY_NONE_PIN[JOY];
             break;
         }
         JoyPinsMask[JOY] |= io_init_structure.Pin;
         io_init_structure.Mode = (JoyMode == JOY_MODE_GPIO) ? IO_MODE_INPUT : IO_MODE_IT_LOW_LEVEL;
         io_init_structure.Pull = IO_PULLUP;
         ret = BSP_IO_Init(0, &io_init_structure);
         if ((ret == BSP_ERROR_NONE) && (JoyMode == JOY_MODE_EXTI))
         {
           /* Interrupt already enabled on BSP_IO_Init, just register callback */
           if (HAL_EXTI_GetHandle(&hjoy_exti[JOY], JOY_EXTI_LINE[JOY]) == HAL_OK)
           {
             if (HAL_EXTI_RegisterCallback(&hjoy_exti[JOY], HAL_EXTI_COMMON_CB_ID, JoyCallback[JOY]) != HAL_OK)
             {
               ret = BSP_ERROR_PERIPH_FAILURE;
             }
           }
           else
           {
             ret = BSP_ERROR_PERIPH_FAILURE;
           }
         }
  }
  return ret;
}

/**
  * @brief  DeInit joystick GPIOs.
  * @note   JOY DeInit does not disable the MFX, just set the MFX pins in Off mode
  * @param  JOY Joystick to be de-initialized
  * @param  JoyPins joystick pins to be de-initialized
  * @retval BSP status
  */
int32_t BSP_JOY_DeInit(JOY_TypeDef JOY, JOYPin_TypeDef JoyPins)
{
  int32_t ret = BSP_ERROR_NONE;
  BSP_IO_Init_t io_init_structure;

  if(JOY >= JOYn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
        switch (JoyPins)
        {
         case JOY_SEL:
          io_init_structure.Pin = JOY_SEL_PIN[JOY];
          CurrentJoyPins[JOY] &= ~(JOY_SEL_PIN[JOY]);
          break;
         case JOY_DOWN:
          io_init_structure.Pin = JOY_DOWN_PIN[JOY];
          CurrentJoyPins[JOY] &= ~(JOY_DOWN_PIN[JOY]);
          break;
         case JOY_LEFT:
          io_init_structure.Pin = JOY_LEFT_PIN[JOY];
          CurrentJoyPins[JOY] &= ~(JOY_LEFT_PIN[JOY]);
          break;
         case JOY_RIGHT:
          io_init_structure.Pin = JOY_RIGHT_PIN[JOY];
          CurrentJoyPins[JOY] &= ~(JOY_RIGHT_PIN[JOY]);
          break;
         case JOY_UP:
          io_init_structure.Pin = JOY_UP_PIN[JOY];
          CurrentJoyPins[JOY] &= ~(JOY_UP_PIN[JOY]);
          break;
         case JOY_ALL:
          io_init_structure.Pin = JOY_ALL_PIN[JOY];
          CurrentJoyPins[JOY] &= ~(JOY_ALL_PIN[JOY]);
          break;
         default :
          CurrentJoyPins[JOY] &= ~(JOY_NONE_PIN[JOY]);
          break;
        }

        io_init_structure.Pull = IO_PULLUP;
        io_init_structure.Mode = IO_MODE_OFF;
        JoyPinsMask[JOY] &= ~io_init_structure.Pin;

        if(BSP_IO_Init(0, &io_init_structure) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_BUS_FAILURE;
        }
  }
  return ret;
}

/**
  * @brief  Returns the current joystick status.
  * @param  JOY Joystick instance
  * @retval Code of the joystick key pressed
  *          This code can be one of the following values:
  *            @arg  JOY1_NONE
  *            @arg  JOY1_SEL
  *            @arg  JOY1_DOWN
  *            @arg  JOY1_LEFT
  *            @arg  JOY1_RIGHT
  *            @arg  JOY1_UP
  */
int32_t BSP_JOY_GetState(JOY_TypeDef JOY)
{
  int32_t ret = BSP_ERROR_NONE;
  int32_t status;

  if(JOY >= JOYn)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
      if ((CurrentJoyPins[JOY] & JOY_SEL_PIN[JOY]) == JOY_SEL_PIN[JOY])
      {
        /* Read status of JOY_SEL */
        status = BSP_IO_ReadPin(0, JOY_SEL_PIN[JOY]);
        if (status >= 0)
        {
          ret = (status == (int32_t)IO_PIN_RESET) ? (int32_t)JOY_SEL : (int32_t)JOY_NONE;
        }
        else
        {
          ret = status;
        }
      }
      if (((CurrentJoyPins[JOY] & JOY_UP_PIN[JOY]) == JOY_UP_PIN[JOY]) && (ret == (int32_t)JOY_NONE))
      {
        /* Read status of JOY_UP */
        status = BSP_IO_ReadPin(0, JOY_UP_PIN[JOY]);
        if (status >= 0)
        {
          ret = (status == (int32_t)IO_PIN_RESET) ? (int32_t)JOY_UP : (int32_t)JOY_NONE;
        }
        else
        {
          ret = status;
        }
      }
      if (((CurrentJoyPins[JOY] & JOY_DOWN_PIN[JOY]) == JOY_DOWN_PIN[JOY]) && (ret == (int32_t)JOY_NONE))
      {
        /* Read status of JOY_DOWN */
        status = BSP_IO_ReadPin(0, JOY_DOWN_PIN[JOY]);
        if (status >= 0)
        {
          ret = (status == (int32_t)IO_PIN_RESET) ? (int32_t)JOY_DOWN : (int32_t)JOY_NONE;
        }
        else
        {
          ret = status;
        }
      }
      if (((CurrentJoyPins[JOY] & JOY_LEFT_PIN[JOY]) == JOY_LEFT_PIN[JOY]) && (ret == (int32_t)JOY_NONE))
      {
        /* Read status of JOY_LEFT */
        status = BSP_IO_ReadPin(0, JOY_LEFT_PIN[JOY]);
        if (status >= 0)
        {
          ret = (status == (int32_t)IO_PIN_RESET) ? (int32_t)JOY_LEFT : (int32_t)JOY_NONE;
        }
        else
        {
          ret = status;
        }
      }
      if (((CurrentJoyPins[JOY] & JOY_RIGHT_PIN[JOY]) == JOY_RIGHT_PIN[JOY]) && (ret == (int32_t)JOY_NONE))
      {
    /* Read status of JOY_RIGHT */
    status = BSP_IO_ReadPin(0, JOY_RIGHT_PIN[JOY]);
    if (status >= 0)
    {
      ret = (status == (int32_t)IO_PIN_RESET) ? (int32_t)JOY_RIGHT : (int32_t)JOY_NONE;
    }
    else
    {
      ret = status;
    }
  }
  }
  return ret;
}

/**
  * @brief  This function handles EXTI_LINE_0 interrupt request.
  * @retval None
  */
void BSP_JOY_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hjoy_exti[JOY1]);
}

/**
  * @brief  BSP Joystick Callback.
  * @param  JOY Joystick instance
  * @param  JoyPin Specifies the pin connected EXTI line
  * @retval None.
  */
__weak void BSP_JOY_Callback(JOY_TypeDef JOY, JOYPin_TypeDef JoyPin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(JOY);
  UNUSED(JoyPin);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on JoyPin is triggered. */
}
#endif /* USE_BSP_JOY_FEATURE */
#endif /* USE_BSP_IO_CLASS */

/**
  * @}
  */ 

/** @defgroup STM32G474E-EVAL_Private_Functions Private Functions
  * @{
  */ 
/*******************************************************************************
                            Static Functions
*******************************************************************************/
#if (USE_BSP_IO_CLASS > 0)
#if (USE_BSP_JOY_FEATURE > 0)
/**
  * @brief  Joystick EXTI line detection callbacks.
  * @retval None
  */
static void JOY1_EXTI_Callback(void)
{
  if ((CurrentJoyPins[JOY1] & JOY_SEL_PIN[JOY1]) == JOY_SEL_PIN[JOY1])
  {
    /* Read IT status of JOY_SEL */
    if (BSP_IO_GetIT(0, JOY_SEL_PIN[JOY1]) == (int32_t)IO_PIN_IT_SET)
    {
      BSP_JOY_Callback(JOY1, JOY_SEL);
      /* Clear IT status of JOY_SEL */
      (void)BSP_IO_ClearIT(0, JOY_SEL_PIN[JOY1]);
    }
  }
  if ((CurrentJoyPins[JOY1] & JOY_UP_PIN[JOY1]) == JOY_UP_PIN[JOY1])
  {
    /* Read IT status of JOY_UP */
    if (BSP_IO_GetIT(0, JOY_UP_PIN[JOY1]) == (int32_t)IO_PIN_IT_SET)
    {
      BSP_JOY_Callback(JOY1, JOY_UP);
      /* Clear IT status of JOY_UP */
      (void)BSP_IO_ClearIT(0, JOY_UP_PIN[JOY1]);
    }
  }
  if ((CurrentJoyPins[JOY1] & JOY_DOWN_PIN[JOY1]) == JOY_DOWN_PIN[JOY1])
  {
    /* Read IT status of JOY_DOWN */
    if (BSP_IO_GetIT(0, JOY_DOWN_PIN[JOY1]) == (int32_t)IO_PIN_IT_SET)
    {
      BSP_JOY_Callback(JOY1, JOY_DOWN);
      /* Clear IT status of JOY_DOWN */
      (void)BSP_IO_ClearIT(0, JOY_DOWN_PIN[JOY1]);
    }
  }
  if ((CurrentJoyPins[JOY1] & JOY_LEFT_PIN[JOY1]) == JOY_LEFT_PIN[JOY1])
  {
    /* Read IT status of JOY_LEFT */
    if (BSP_IO_GetIT(0, JOY_LEFT_PIN[JOY1]) == (int32_t)IO_PIN_IT_SET)
    {
      BSP_JOY_Callback(JOY1, JOY_LEFT);
      /* Clear IT status of JOY_LEFT */
      (void)BSP_IO_ClearIT(0, JOY_LEFT_PIN[JOY1]);
    }
  }
  if ((CurrentJoyPins[JOY1] & JOY_RIGHT_PIN[JOY1]) == JOY_RIGHT_PIN[JOY1])
  {
    /* Read IT status of JOY_RIGHT */
    if (BSP_IO_GetIT(0, JOY_RIGHT_PIN[JOY1]) == (int32_t)IO_PIN_IT_SET)
    {
      BSP_JOY_Callback(JOY1, JOY_RIGHT);
      /* Clear IT status of JOY_RIGHT */
      (void)BSP_IO_ClearIT(0, JOY_RIGHT_PIN[JOY1]);
    }
  }
}
#endif /* USE_BSP_JOY_FEATURE */
#endif /* USE_BSP_IO_CLASS */

#if (USE_BSP_COM_FEATURE > 0)
/**
  * @brief  Initializes USART1 MSP.
  * @param  huart UART handle
  * @retval None
  */
static void USART1_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* Enable GPIO clock */
  COM1_TX_GPIO_CLK_ENABLE();
  COM1_RX_GPIO_CLK_ENABLE();

  /* Enable USART clock */
  COM1_CLK_ENABLE();

  /* Configure USART Tx as alternate function */
  gpio_init_structure.Pin = COM1_TX_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Alternate = COM1_TX_AF;
  HAL_GPIO_Init(COM1_TX_GPIO_PORT, &gpio_init_structure);

  /* Configure USART Rx as alternate function */
  gpio_init_structure.Pin = COM1_RX_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_PP;
  gpio_init_structure.Alternate = COM1_RX_AF;
  HAL_GPIO_Init(COM1_RX_GPIO_PORT, &gpio_init_structure);
}

/**
  * @brief  Initialize USART1 Msp part
  * @param  huart UART handle
  * @retval None
  */
static void USART1_MspDeInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef          gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* COM GPIO pin configuration */
  gpio_init_structure.Pin  = COM1_TX_PIN;
  HAL_GPIO_DeInit(COM1_TX_GPIO_PORT, gpio_init_structure.Pin);

  gpio_init_structure.Pin  = COM1_RX_PIN;
  HAL_GPIO_DeInit(COM1_RX_GPIO_PORT, gpio_init_structure.Pin);

  /* Disable USART clock */
  COM1_CLK_DISABLE();
}
#endif /* #if USE_BSP_COM_FEATURE */

#if (USE_BSP_POT_FEATURE > 0)
/**
  * @brief  Initialize POT1 Msp part
  * @param  hadc ADC handle
  * @retval None
  */
static void ADC5_MspInit(ADC_HandleTypeDef *hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* ADC5 clock enable */
  POT1_CLK_ENABLE();
}

/**
  * @brief  De-Initialize POT1 Msp part
  * @param  hadc ADC handle
  * @retval None
  */
static void ADC5_MspDeInit(ADC_HandleTypeDef *hadc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hadc);

  /* Disable ADC clock */
  POT1_CLK_DISABLE();
}

#if (USE_BSP_POT_COMP_FEATURE > 0)
/**
  * @brief  Initialize POT1 Msp part
  * @param  hcomp COMP handle
  * @retval None
  */
static void COMP6_MspInit(COMP_HandleTypeDef *hcomp)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcomp);

  /* Comp6 clock enable */
  POT1_GPIO_CLK_ENABLE();

  GPIO_InitTypeDef  gpio_init_structure;

  /* Comp6 Channel GPIO pin configuration */
  gpio_init_structure.Pin  = POT1_VINP_GPIO_PIN;
  gpio_init_structure.Mode = GPIO_MODE_ANALOG;
  gpio_init_structure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(POT1_VINP_GPIO_PORT, &gpio_init_structure);

  gpio_init_structure.Pin       = POT1_OUT_GPIO_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_AF_OD;
  gpio_init_structure.Alternate = GPIO_AF7_COMP6;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  HAL_GPIO_Init(POT1_OUT_GPIO_PORT, &gpio_init_structure);
}

/**
  * @brief  De-Initialize POT1 Msp part
  * @param  hcomp COMP handle
  * @retval None
  */
static void COMP6_MspDeInit(COMP_HandleTypeDef *hcomp)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcomp);

  GPIO_InitTypeDef  gpio_init_structure;

  /* POT GPIO pin configuration */
  /* do not deconfig POT1_VINP_GPIO_PORT as used by OPAMP4 */

  gpio_init_structure.Pin  = POT1_OUT_GPIO_PIN;
  HAL_GPIO_DeInit(POT1_OUT_GPIO_PORT, gpio_init_structure.Pin);
}
#endif /* (USE_BSP_POT_COMP_FEATURE > 0) */

/**
  * @brief  Initialize POT1 Msp part
  * @param  hopamp OPAMP handle
  * @retval None
  */
static void OPAMP4_MspInit(OPAMP_HandleTypeDef *hopamp)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hopamp);

  /* OpAmp4 clock enable */
  POT1_GPIO_CLK_ENABLE();

  GPIO_InitTypeDef  gpio_init_structure;

  /* OpAmp4 Channel GPIO pin configuration : analog by default */
  gpio_init_structure.Pin  = POT1_INM_GPIO_PIN;
  gpio_init_structure.Mode = GPIO_MODE_ANALOG;
  gpio_init_structure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(POT1_INM_GPIO_PORT, &gpio_init_structure);

  gpio_init_structure.Pin  = POT1_VINP_GPIO_PIN;
  gpio_init_structure.Mode = GPIO_MODE_ANALOG;
  gpio_init_structure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(POT1_VINP_GPIO_PORT, &gpio_init_structure);

  gpio_init_structure.Pin       = POT1_VOUT_GPIO_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_ANALOG;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  HAL_GPIO_Init(POT1_VOUT_GPIO_PORT, &gpio_init_structure);
}

/**
  * @brief  De-Initialize POT1 Msp part
  * @param  hopamp OPAMP handle
  * @retval None
  */
static void OPAMP4_MspDeInit(OPAMP_HandleTypeDef *hopamp)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hopamp);

  GPIO_InitTypeDef  gpio_init_structure;

  /* POT GPIO pin configuration */
  gpio_init_structure.Pin  = POT1_VOUT_GPIO_PIN;
  HAL_GPIO_DeInit(POT1_VOUT_GPIO_PORT, gpio_init_structure.Pin);
  /* do not deconfig POT1_VINP_GPIO_PORT as used by COMP6 */
  gpio_init_structure.Pin  = POT1_INM_GPIO_PIN;
  HAL_GPIO_DeInit(POT1_INM_GPIO_PORT, gpio_init_structure.Pin);
}

#endif /*(USE_BSP_POT_FEATURE > 0)*/
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
