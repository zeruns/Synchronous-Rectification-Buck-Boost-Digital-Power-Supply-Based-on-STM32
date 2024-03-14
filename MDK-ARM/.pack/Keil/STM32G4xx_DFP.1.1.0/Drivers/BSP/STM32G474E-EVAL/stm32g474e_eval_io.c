/**
  ******************************************************************************
  * @file    stm32g474e_eval_io.c
  * @author  MCD Application Team
  * @brief   This file provides a set of functions needed to manage the IO pins
  *          on STM32G474E-EVAL board.
  @verbatim
  How To use this driver:
  -----------------------
   - This driver is used to drive the IO module of the STM32G474E-EVAL board.
   - The MFXSTM32L152 IO expander device component driver must be included with this
     driver in order to run the IO functionalities commanded by the IO expander.
     device mounted on the board.

  Driver description:
  -------------------
  + Initialization steps:
     o Initialize the IO module using the BSP_IO_Init() function. This
       function includes the MSP layer hardware resources initialization and the
       communication layer configuration to start the IO functionalities use.

  + IO functionalities use
     o The IO pin mode is configured when calling the function BSP_IO_Init(), you
       must specify the desired IO mode by choosing the "Mode" parameter of the
       BSP_IO_Init_t structure.
     o If an IO pin is used in interrupt mode, the function BSP_IO_GetIT() is
       needed to get the interrupt status. To clear the IT pending bits, you should
       call the function BSP_IO_ClearIT() with specifying the IO pending bit to clear.
     o The IT is handled using the corresponding external interrupt IRQ handler,
       the user IT callback treatment is implemented on the same external interrupt
       callback.
     o To get/set an IO pin combination state you can use the functions
       BSP_IO_ReadPin()/BSP_IO_WritePin() or the function BSP_IO_TogglePin() to toggle the pin
       state.
  @endverbatim
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
#include "stm32g474e_eval_io.h"
#include "stm32g474e_eval_bus.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @defgroup STM32G474E-EVAL_IO STM32G474E-EVAL IO
  * @brief    This file includes the IO Expander driver of STM32G474E-EVAL boards.
  * @{
  */

/** @defgroup STM32G474E-EVAL_IO_Exported_Variables Exported Variables
  * @{
  */
IOEXPANDER_Ctx_t                IOECtx[IOEXPANDER_INSTANCES_NBR] = {0};
MFXSTM32L152_Object_t           IOECompObj[IOEXPANDER_INSTANCES_NBR] = {NULL};
IDD_Drv_t                       *IddDrv[IOEXPANDER_INSTANCES_NBR] = {NULL};
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_IO_Private_Variables Private Variables
  * @{
  */
static IO_Drv_t                 *IoDrv[IOEXPANDER_INSTANCES_NBR] = {NULL};
/**
  * @}
  */

/** STM32G474E-EVAL_IO_Private_FunctionPrototypes Private Functions
  */
static int32_t MFXSTM32L152_Probe(uint32_t Instance);
/**
  */

/** @addtogroup STM32G474E-EVAL_IO_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes the IO peripheral according to the specified parameters in the BSP_IO_Init_t.
  * @param  Instance IOE instance
  * @param  Init     pointer to a BSP_IO_Init_t structure that contains
  *         the configuration information for the specified IO pin.
  * @retval BSP status
  */
int32_t BSP_IO_Init(uint32_t Instance, BSP_IO_Init_t *Init)
{
  int32_t ret = BSP_ERROR_NONE;

  if((Instance >= IOEXPANDER_INSTANCES_NBR) || (Init == NULL))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((IOECtx[Instance].Functions & IOEXPANDER_IO_MODE) != IOEXPANDER_IO_MODE)
    {
      if(BSP_IOEXPANDER_Init(Instance, IOEXPANDER_IO_MODE) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else if (IoDrv[Instance]->Start(&IOECompObj[Instance], IO_PIN_ALL) < 0)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        IOECtx[Instance].Functions |= IOEXPANDER_IO_MODE;
      }
    }

    if(ret == BSP_ERROR_NONE)
    {
      /* If IT mode is selected, configures MFX low level interrupt */
      if(Init->Mode >= IO_MODE_IT_RISING_EDGE)
      {
        BSP_IOEXPANDER_ITConfig();
      }

      /* Initializes IO pin */
      if(IoDrv[Instance]->Init(&IOECompObj[Instance], Init) < 0)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
    }
  }

  return ret;
}

/**
  * @brief  DeInitializes the IO peripheral
  * @param  Instance IOE instance
  * @retval BSP status
  */
int32_t BSP_IO_DeInit(uint32_t Instance)
{
  int32_t ret;

  if (Instance >= IOEXPANDER_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if ((IOECtx[Instance].Functions & IOEXPANDER_IO_MODE) != IOEXPANDER_IO_MODE)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    IOECtx[Instance].Functions &= ~(IOEXPANDER_IO_MODE);
    ret = BSP_IOEXPANDER_DeInit(Instance);
  }

  return ret;
}

/**
  * @brief  Gets the selected pins IT status.
  * @param  Instance IOE instance
  * @param  IoPin  Selected pins to check the status.
  *          This parameter can be any combination of the IO pins.
  * @retval Pin IT status or BSP_ERROR_WRONG_PARAM
  */
int32_t BSP_IO_GetIT(uint32_t Instance, uint32_t IoPin)
{
  int32_t ret;

  if((Instance >= IOEXPANDER_INSTANCES_NBR) || ((IOECtx[Instance].Functions & IOEXPANDER_IO_MODE) == 0U))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Return the IO Pin IT status */
    ret = IoDrv[Instance]->ITStatus(&IOECompObj[Instance], IoPin);
    if (ret < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else if (ret == 0)
    {
      ret = (int32_t)IO_PIN_IT_RESET;
    }
    else
    {
      ret = (int32_t)IO_PIN_IT_SET;
    }
  }

  return ret;
}

/**
  * @brief  Clear only one or a selection of IO IT pending bits.
  * @param  Instance IOE instance
  * @param  IO_Pins_To_Clear   IoExpander IRQ status IO pin to clear (or combination of several IOs)
  * @retval BSP status
  */
int32_t BSP_IO_ClearIT(uint32_t Instance, uint32_t IO_Pins_To_Clear)
{
  int32_t ret = BSP_ERROR_NONE;

  if((Instance >= IOEXPANDER_INSTANCES_NBR) || ((IOECtx[Instance].Functions & IOEXPANDER_IO_MODE) == 0U))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Clear only the selected list of IO IT pending bits */
    if(IoDrv[Instance]->ClearIT(&IOECompObj[Instance], IO_Pins_To_Clear) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  Sets the selected pins state.
  * @param  Instance IOE instance
  * @param  IoPin  Selected pins to write.
  *          This parameter can be any combination of the IO pins.
  * @param  PinState  New pins state to write
  * @retval BSP status
  */
int32_t BSP_IO_WritePin(uint32_t Instance, uint32_t IoPin, uint32_t PinState)
{
  int32_t ret = BSP_ERROR_NONE;

  if((Instance >= IOEXPANDER_INSTANCES_NBR) || ((IOECtx[Instance].Functions & IOEXPANDER_IO_MODE) == 0U))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Set the Pin state */
    if(IoDrv[Instance]->WritePin(&IOECompObj[Instance], IoPin, PinState) < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
  }

  return ret;
}

/**
  * @brief  Gets the selected pins current state.
  * @param  Instance IOE instance
  * @param  IoPin  Selected pin to read.
  * @retval The current pins state or BSP_ERROR_WRONG_PARAM
  */
int32_t BSP_IO_ReadPin(uint32_t Instance, uint32_t IoPin)
{
  int32_t ret;

  if((Instance >= IOEXPANDER_INSTANCES_NBR) || ((IOECtx[Instance].Functions & IOEXPANDER_IO_MODE) == 0U))
  {
    ret =  BSP_ERROR_WRONG_PARAM;
  }
  else
  {
     ret = IoDrv[Instance]->ReadPin(&IOECompObj[Instance], IoPin);
     if (ret < 0)
     {
      ret = BSP_ERROR_COMPONENT_FAILURE;
     }
    else if (ret == 0)
    {
      ret = (int32_t)IO_PIN_RESET;
    }
    else
    {
      ret = (int32_t)IO_PIN_SET;
    }
  }
  return ret;
}

/**
  * @brief  Toggles the selected pins state.
  * @param  Instance IOE instance
  * @param  IoPin  Selected pins to toggle.
  *          This parameter can be any combination of the IO pins.
  * @note   This function is only used to toggle one pin in the same time
  * @retval None
  */
int32_t BSP_IO_TogglePin(uint32_t Instance, uint32_t IoPin)
{
  int32_t ret = BSP_ERROR_NONE;
  int32_t pinState;

  if((Instance >= IOEXPANDER_INSTANCES_NBR) || ((IOECtx[Instance].Functions & IOEXPANDER_IO_MODE) == 0U))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Toggle the current pin state */
    pinState = IoDrv[Instance]->ReadPin(&IOECompObj[Instance], IoPin);
    if (pinState < 0)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      if(pinState == 0) /* Reset state */
      {
        if(IoDrv[Instance]->WritePin(&IOECompObj[Instance], IoPin, IO_PIN_SET) < 0)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
      }
      else /* Set state */
      {
        if(IoDrv[Instance]->WritePin(&IOECompObj[Instance], IoPin, IO_PIN_RESET) < 0)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
      }
    }
  }

  return ret;
}

/**
  * @brief  Initializes and start the IOExpander component
  * @param  Instance IOE instance
  * @param  Function  to be initialized. Could be IOEXPANDER_IO_MODE or IOEXPANDER_IDD_MODE
  * @note   IOEXPANDER_TS_MODE is not used on STM32G474E-EVAL board
  * @retval BSP status
  */
int32_t BSP_IOEXPANDER_Init(uint32_t Instance, uint32_t Function)
{
  int32_t ret = BSP_ERROR_NONE;

  if ((Instance >= IOEXPANDER_INSTANCES_NBR) || (Function > IOEXPANDER_IDD_MODE))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(IOECtx[Instance].IsInitialized == 0U)
    {
      if(MFXSTM32L152_Probe(Instance) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_NO_INIT;
      }
      else
      {
        IOECtx[Instance].IsInitialized = 1;
      }
    }

    if(IOECtx[Instance].IsInitialized == 1U)
    {
      if (Function == IOEXPANDER_IO_MODE)
      {
        IoDrv[Instance] = (IO_Drv_t *)&MFXSTM32L152_IO_Driver;
      }
      else if (Function == IOEXPANDER_IDD_MODE)
      {
        IddDrv[Instance] = (IDD_Drv_t *)&MFXSTM32L152_IDD_Driver;
      }
      else
      {
        ret = BSP_ERROR_WRONG_PARAM;
      }
    }
  }
  return ret;
}

/**
  * @brief  De-Initializes the IOExpander component
  * @param  Instance IOE instance
  * @note   The de-init is common for all IOE functions
  * @retval BSP status
  */
int32_t BSP_IOEXPANDER_DeInit(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  if (Instance >= IOEXPANDER_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (IOECtx[Instance].IsInitialized == 1U)
    {
      /* DeInit MFX if no more Functions on are going */
      if (IOECtx[Instance].Functions == 0U)
      {
        if (IoDrv[Instance]->DeInit(&IOECompObj[Instance]) < 0)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          IOECtx[Instance].IsInitialized = 0;
        }
      }
    }
  }

  return ret;
}

/**
  * @brief  Configures IoExpander low level interrupt.
  * @retval None
  */
void BSP_IOEXPANDER_ITConfig(void)
{
  static uint32_t ioexpander_it_enabled = 0U;
  GPIO_InitTypeDef  gpio_init_structure;

  if(ioexpander_it_enabled == 0U)
  {
    ioexpander_it_enabled = 1U;
    /* Enable the GPIO EXTI clock */
    IOEXPANDER_IRQOUT_GPIO_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    gpio_init_structure.Pin   = IOEXPANDER_IRQOUT_GPIO_PIN;
    gpio_init_structure.Pull  = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init_structure.Mode  = GPIO_MODE_IT_RISING;
    HAL_GPIO_Init(IOEXPANDER_IRQOUT_GPIO_PORT, &gpio_init_structure);

    /* Enable and set GPIO EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((IRQn_Type)(IOEXPANDER_IRQOUT_EXTI_IRQn), BSP_IOEXPANDER_IT_PRIORITY, 0x0F);
    HAL_NVIC_EnableIRQ((IRQn_Type)(IOEXPANDER_IRQOUT_EXTI_IRQn));
  }
}

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_IO_Private_Functions Private Functions
  * @{
  */

/**
  * @brief  Register Bus IOs if component ID is OK
  * @retval error status
  */
static int32_t MFXSTM32L152_Probe(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;
  MFXSTM32L152_IO_t IOCtx;
  uint32_t mfxstm32l152_id,i;
  uint8_t const i2c_address[] = {IO_I2C_ADDRESS, IO_I2C_ADDRESS_2};

  IOCtx.Init        = BSP_I2C3_Init;
  IOCtx.DeInit      = BSP_I2C3_DeInit;
  IOCtx.ReadReg     = BSP_I2C3_ReadReg;
  IOCtx.WriteReg    = BSP_I2C3_WriteReg;
  IOCtx.GetTick     = BSP_GetTick;

  for(i = 0U; i < sizeof(i2c_address); i++)
  {
    IOCtx.Address     = (uint16_t)i2c_address[i];

    if(MFXSTM32L152_RegisterBusIO (&IOECompObj[Instance], &IOCtx) != MFXSTM32L152_OK)
    {
     ret = BSP_ERROR_BUS_FAILURE;
    }
    else if(MFXSTM32L152_ReadID(&IOECompObj[Instance], &mfxstm32l152_id) != MFXSTM32L152_OK)
    {
         ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
        if((mfxstm32l152_id == MFXSTM32L152_ID) || (mfxstm32l152_id == MFXSTM32L152_ID_2))
        {
          if(MFXSTM32L152_Init(&IOECompObj[Instance]) != MFXSTM32L152_OK)
          {
            ret = BSP_ERROR_COMPONENT_FAILURE;
          }
          else
          {
            ret = BSP_ERROR_NONE;
            break;
          }
        }
        else
        {
          ret = BSP_ERROR_UNKNOWN_COMPONENT;
        }
    }
  }

  return ret;
}

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
