/**
  ******************************************************************************
  * @file    stm32g474e_eval_sram.c
  * @author  MCD Application Team
  * @brief   This file includes the SRAM driver for the IS61WV51216BLL-10MLI memory
  *          device mounted on STM32G474E-EVAL evaluation boards.
  @verbatim
  How To use this driver:
  -----------------------
   - This driver is used to drive the IS61WV51216BLL-10MLI SRAM external memory mounted
     on STM32G474E-EVAL evaluation board.
   - This driver does not need a specific component driver for the SRAM device
     to be included with.

  Driver description:
  ------------------
  + Initialization steps:
     o Initialize the SRAM external memory using the BSP_SRAM_Init() function. This
       function includes the MSP layer hardware resources initialization and the
       FMC controller configuration to interface with the external SRAM memory.

  + SRAM read/write operations
     o SRAM external memory can be accessed with read/write operations once it is
       initialized.
       Read/write operation can be performed with AHB access using the HAL functions
       HAL_SRAM_Read_Xb()/HAL_SRAM_Write_Xb() (with X=8,16 or 32),
       or by DMA transfer using the functions
       HAL_SRAM_Read_DMA()/HAL_SRAM_Write_DMA().
     o The AHB access is performed with 16-bit width transaction, the DMA transfer
       configuration is fixed at single (no burst) halfword transfer
       (see the SRAM_MspInit() static function).
     o User can implement his own BSP functions for read/write access with his desired
       configurations.
     o If interrupt mode is used for DMA transfer, the function BSP_SRAM_DMA_IRQHandler()
       is called in IRQ handler file, to serve the generated interrupt once the DMA
       transfer is complete.
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
#include "stm32g474e_eval_sram.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @defgroup STM32G474E-EVAL_SRAM STM32G474E-EVAL SRAM
  * @brief      This file includes the SRAM driver of
  *             STM32G474E-EVAL boards.
  * @{
  */

/** @defgroup STM32G474E-EVAL_SRAM_Exported_Variables Exported Variables
  * @{
  */
SRAM_HandleTypeDef hsram;
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SRAM_Private_Variables Private Variables
  * @{
  */
#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
static uint32_t IsMspCallbacksValid = 0;
#endif
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SRAM_Private_Constants Private Constants
  * @{
  */
/* Timings for SRAM IS61WV51216BLL-10MLI with system clock up to 170Mhz */
#define SRAM_ADDR_SETUP_TIME       2        /* 10ns with a clock at 170 MHz (period of 5.88 ns) */
#define SRAM_DATA_SETUP_TIME       2        /* 6ns  with a clock at 170 MHz (period of 5.88 ns) */
#define SRAM_DATA_HOLD_TIME        0
#define SRAM_TURN_AROUND_TIME      1        /* 4ns  with a clock at 170 MHz (period of 5.88 ns) */
/**
  * @}
  */

/** STM32G474E-EVAL_SRAM_Private_FunctionPrototypes Private Functions
  */
static void SRAM_MspInit(SRAM_HandleTypeDef  *hsram);
static void SRAM_MspDeInit(SRAM_HandleTypeDef  *hsram);
/**
  */

/** @addtogroup STM32G474E-EVAL_SRAM_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes the SRAM device.
  * @param  Instance SRAM instance
  * @retval BSP status
  */
int32_t BSP_SRAM_Init(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= SRAM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 0)
    /* Msp SRAM initialization */
    SRAM_MspInit(&hsram);
#else
    /* Register the SRAM MSP Callbacks */
    if(IsMspCallbacksValid == 0)
    {
      if(BSP_SRAM_RegisterDefaultMspCallbacks(Instance) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_PERIPH_FAILURE;
      }
    }
#endif /* USE_HAL_SRAM_REGISTER_CALLBACKS */

    /* __weak function can be rewritten by the application */
    if(MX_SRAM_BANK1_Init(&hsram) != HAL_OK)
    {
      ret = BSP_ERROR_NO_INIT;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  return ret;
}

/**
  * @brief  DeInitializes the SRAM device.
  * @param  Instance SRAM instance
  * @retval BSP status
  */
int32_t BSP_SRAM_DeInit(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= SRAM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* SRAM device de-initialization */
    hsram.Instance = FMC_NORSRAM_DEVICE;
    hsram.Extended = FMC_NORSRAM_EXTENDED_DEVICE;

    (void)HAL_SRAM_DeInit(&hsram);
#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 0)
      /* SRAM controller de-initialization */
      SRAM_MspDeInit(&hsram);
#endif /* (USE_HAL_SRAM_REGISTER_CALLBACKS == 0) */
      ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
  * @brief  Initializes the SRAM peripheral.
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_SRAM_BANK1_Init(SRAM_HandleTypeDef *hsram)
{
  static FMC_NORSRAM_TimingTypeDef SramTiming;

  /* SRAM device configuration */
  hsram->Instance = FMC_NORSRAM_DEVICE;
  hsram->Extended = FMC_NORSRAM_EXTENDED_DEVICE;

  /* SRAM device configuration */
  hsram->Init.NSBank             = FMC_NORSRAM_BANK1;
  hsram->Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE;
  hsram->Init.MemoryType         = FMC_MEMORY_TYPE_SRAM;
  hsram->Init.MemoryDataWidth    = FMC_NORSRAM_MEM_BUS_WIDTH_16;
  hsram->Init.BurstAccessMode    = FMC_BURST_ACCESS_MODE_DISABLE;
  hsram->Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram->Init.WaitSignalActive   = FMC_WAIT_TIMING_BEFORE_WS;
  hsram->Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;
  hsram->Init.WaitSignal         = FMC_WAIT_SIGNAL_DISABLE;
  hsram->Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;
  hsram->Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram->Init.WriteBurst         = FMC_WRITE_BURST_DISABLE;
  hsram->Init.ContinuousClock    = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;
  hsram->Init.WriteFifo          = FMC_WRITE_FIFO_DISABLE;
  hsram->Init.NBLSetupTime       = FMC_NBL_SETUPTIME_0;
  hsram->Init.PageSize           = FMC_PAGE_SIZE_NONE;

  /* Timing configuration */
  SramTiming.AddressSetupTime      = SRAM_ADDR_SETUP_TIME;
  SramTiming.AddressHoldTime       = 1;                           /* Min value, don't care on SRAM Access mode A */
  SramTiming.DataSetupTime         = SRAM_DATA_SETUP_TIME;
  SramTiming.DataHoldTime          = SRAM_DATA_HOLD_TIME;
  SramTiming.BusTurnAroundDuration = SRAM_TURN_AROUND_TIME;
  SramTiming.CLKDivision           = 2;                           /* Min value, don't care on SRAM Access mode A */
  SramTiming.DataLatency           = 2;                           /* Min value, don't care on SRAM Access mode A */
  SramTiming.AccessMode            = FMC_ACCESS_MODE_A;

  /* SRAM controller initialization */
  if(HAL_SRAM_Init(hsram, &SramTiming, NULL) != HAL_OK)
  {
    return  HAL_ERROR;
  }
  return HAL_OK;
}

#if (USE_HAL_SRAM_REGISTER_CALLBACKS == 1)
/**
  * @brief Default BSP SRAM Msp Callbacks
  * @param Instance      SRAM Instance
  * @retval BSP status
  */
int32_t BSP_SRAM_RegisterDefaultMspCallbacks (uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= SRAM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_SRAM_RegisterCallback(&hsram, HAL_SRAM_MSP_INIT_CB_ID, SRAM_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SRAM_RegisterCallback(&hsram, HAL_SRAM_MSP_DEINIT_CB_ID, SRAM_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsMspCallbacksValid = 1;
    }
  }
  /* Return BSP status */
  return ret;
}

/**
  * @brief BSP SRAM Msp Callback registering
  * @param Instance     SRAM Instance
  * @param CallBacks    pointer to MspInit/MspDeInit callbacks functions
  * @retval BSP status
  */
int32_t BSP_SRAM_RegisterMspCallbacks (uint32_t Instance, BSP_SRAM_Cb_t *CallBacks)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= SRAM_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_SRAM_RegisterCallback(&hsram, HAL_SRAM_MSP_INIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_SRAM_RegisterCallback(&hsram, HAL_SRAM_MSP_DEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      IsMspCallbacksValid = 1;
    }
  }
  /* Return BSP status */
  return ret;
}
#endif /* (USE_HAL_SRAM_REGISTER_CALLBACKS == 1) */

/**
  * @brief  This function handles SRAM DMA interrupt request.
  * @retval None
  */
void BSP_SRAM_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hsram.hdma);
}
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SRAM_Private_Functions Private Functions
  * @{
  */

/**
  * @brief  Initializes SRAM MSP.
  * @param  hsram  SRAM handle
  * @retval None
  */
static void SRAM_MspInit(SRAM_HandleTypeDef  *hsram)
{
  static DMA_HandleTypeDef dma_handle;
  GPIO_InitTypeDef gpio_init_structure;

  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable chosen DMAx clock */
  SRAM_DMAx_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* Common GPIO configuration */
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF12_FMC;

  /*## Data Bus #######*/
  /* GPIOD configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 |
                              GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  /* GPIOE configuration */
  gpio_init_structure.Pin   = GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 |
                              GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  /*## Address Bus #######*/
  /* GPIOD configuration */
  gpio_init_structure.Pin   = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  /* GPIOF configuration */
  gpio_init_structure.Pin   = GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  | GPIO_PIN_5  | GPIO_PIN_7  |
                              GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOF, &gpio_init_structure);

  /* GPIOG configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  |
                              GPIO_PIN_5;
  HAL_GPIO_Init(GPIOG, &gpio_init_structure);

  /*## NOE and NWE configuration #######*/
  gpio_init_structure.Pin =   GPIO_PIN_4  | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  /*## NE1 configuration #######*/
  gpio_init_structure.Pin =   GPIO_PIN_7;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  /*## NBL0, NBL1 configuration #######*/
  gpio_init_structure.Pin =   GPIO_PIN_0  | GPIO_PIN_1;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  /* Configure common DMA parameters */
  dma_handle.Init.Direction           = DMA_MEMORY_TO_MEMORY;
  dma_handle.Init.PeriphInc           = DMA_PINC_ENABLE;
  dma_handle.Init.MemInc              = DMA_MINC_ENABLE;
  dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  dma_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
  dma_handle.Init.Mode                = DMA_NORMAL;
  dma_handle.Init.Priority            = DMA_PRIORITY_HIGH;
  dma_handle.Init.Request             = DMA_REQUEST_MEM2MEM;

  dma_handle.Instance = SRAM_DMAx_CHANNEL;

   /* Associate the DMA handle */
  __HAL_LINKDMA(hsram, hdma, dma_handle);

  /* Deinitialize the Stream for new transfer */
  (void)HAL_DMA_DeInit(&dma_handle);

  /* Configure the DMA Stream */
  (void)HAL_DMA_Init(&dma_handle);

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SRAM_DMAx_IRQn, BSP_SRAM_IT_PRIORITY, 0);
  HAL_NVIC_EnableIRQ(SRAM_DMAx_IRQn);
}

/**
  * @brief  DeInitializes SRAM MSP.
  * @param  hsram  SRAM handle
  * @retval None
  */
static void SRAM_MspDeInit(SRAM_HandleTypeDef  *hsram)
{
  static DMA_HandleTypeDef dma_handle;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsram);

  /*## Data Bus #######*/
  /* GPIOD configuration */
  HAL_GPIO_DeInit(GPIOD,      GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 |
                              GPIO_PIN_14 | GPIO_PIN_15);

  /* GPIOE configuration */
  HAL_GPIO_DeInit(GPIOE,      GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 |
                              GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

  /*## Address Bus #######*/
  /* GPIOD deconfiguration */
  HAL_GPIO_DeInit(GPIOD,      GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);

  /* GPIOF deconfiguration */
  HAL_GPIO_DeInit(GPIOF,      GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  | GPIO_PIN_5  | GPIO_PIN_7  |
                              GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15);

  /* GPIOG deconfiguration */
  HAL_GPIO_DeInit(GPIOG,      GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  |
                              GPIO_PIN_5);

  /*## NOE and NWE configuration #######*/
  HAL_GPIO_DeInit(GPIOD,      GPIO_PIN_4  | GPIO_PIN_5);

  /*## NE1 configuration #######*/
  HAL_GPIO_DeInit(GPIOD,      GPIO_PIN_7);

  /*## NBL0, NBL1 configuration #######*/
  HAL_GPIO_DeInit(GPIOE,      GPIO_PIN_0  | GPIO_PIN_1);

  /* Disable NVIC configuration for DMA interrupt */
  HAL_NVIC_DisableIRQ(SRAM_DMAx_IRQn);

  /* Deinitialize the stream for new transfer */
  dma_handle.Instance = SRAM_DMAx_CHANNEL;
  (void)HAL_DMA_DeInit(&dma_handle);
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
