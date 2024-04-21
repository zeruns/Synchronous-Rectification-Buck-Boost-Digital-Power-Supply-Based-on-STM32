/**
  ******************************************************************************
  * @file    stm32g474e_eval_qspi.c
  * @author  MCD Application Team
  * @brief   This file includes a standard driver for the MT25QL512ABB QSPI
  *          flash memory mounted on STM32G474E-EVAL board.
  ******************************************************************************
  @verbatim
  How To use this driver:
  -----------------------
   - This driver is used to drive the MT25QL512ABB QSPI flash
       memory mounted on STM32G474E-EVAL evaluation board.

   - This driver need a specific component driver (MT25QL512ABB) to be included with.

  Driver description:
  -------------------
   - Initialization steps:
       + Initialize the QPSI external memory using the BSP_QSPI_Init() function. This
            function includes the MSP layer hardware resources initialization and the
            QSPI interface with the external memory.
         STR and DTR transfer rates are supported.
         SPI (1-1-1), SPI 1I2O (1-1-2), SPI 2IO (1-2-2), SPI 1I4O (1-1-4), SPI 4IO (1-4-4),
            DPI (2-2-2) and QPI (4-4-4) modes are supported

   - QSPI memory operations
       + QSPI memory can be accessed with read/write operations once it is
            initialized.
            Read/write operation can be performed with AHB access using the functions
            BSP_QSPI_Read()/BSP_QSPI_Write().
       + The function BSP_QSPI_GetInfo() returns the configuration of the QSPI memory.
            (see the QSPI memory data sheet)
       + Perform erase block operation using the function BSP_QSPI_EraseBlock() and by
            specifying the block address. You can perform an erase operation of the whole
            chip by calling the function BSP_QSPI_EraseChip().
       + The function BSP_QSPI_GetStatus() returns the current status of the QSPI memory.
            (see the QSPI memory data sheet)
       + The function BSP_QSPI_EnableMemoryMappedMode enables the QSPI memory mapped mode
       + The function BSP_QSPI_DisableMemoryMappedMode disables the QSPI memory mapped mode
       + The function BSP_QSPI_ReadID() returns 3 bytes memory IDs: Manufacturer ID,
             Memory type, Memory density
       + The function BSP_QSPI_ConfigFlash() allow to configure the QSPI mode and transfer rate
       + The function BSP_QSPI_SelectFlashID(), in single flash mode, allow to select one
             of the two flash memories available.

  Note:
  --------
    Regarding the "Instance" parameter, needed for all functions, it is used to select
    a QSPI instance. On the STM32G474E-EVAL board, there's one instance on which two
    flash memories are connected. Then, this parameter should be 0.

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
#include "stm32g474e_eval_qspi.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @defgroup STM32G474E-EVAL_QSPI STM32G474E-EVAL QSPI
  * @{
  */

/** @defgroup STM32G474E-EVAL_QSPI_Exported_Variables Exported Variables
  * @{
  */
QSPI_HandleTypeDef hqspi;
QSPI_Ctx_t QSPICtx[QSPI_INSTANCES_NUMBER];
/**
  * @}
  */

/* Private functions ---------------------------------------------------------*/

/** STM32G474E-EVAL_QSPI_Private_FunctionPrototypes Private Functions
  */
static void QSPI_MspInit(QSPI_HandleTypeDef *hqspi);
static void QSPI_MspDeInit(QSPI_HandleTypeDef *hqspi);
static int32_t QSPI_ResetMemory(uint32_t Instance);
static int32_t QSPI_SetODS(uint32_t Instance);
static int32_t QSPI_EnterDPIMode(uint32_t Instance);
static int32_t QSPI_ExitDPIMode(uint32_t Instance);
/**
  */

/** @addtogroup STM32G474E-EVAL_QSPI_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes the QSPI interface.
  * @param  Instance   QSPI Instance
  * @param  Init       QSPI Init structure
  * @retval BSP status
  */
int32_t BSP_QSPI_Init(uint32_t Instance, BSP_QSPI_Init_t *Init)
{
  int32_t ret;
  BSP_QSPI_Info_t pInfo;
  MX_QSPI_Config qspi_config;

  /* Table to handle clock prescalers:
     QSPI freq = SYSCLK /(1 + ClockPrescaler) Mhz
  1: For STR mode to reach max flash clock freq 133Mhz
  2: For DTR mode to reach max flash clock freq 90Mhz
  */
  static const uint32_t PrescalerTab[2] = {1U, 2U};

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check if instance is already initialized */
    if(QSPICtx[Instance].IsInitialized == QSPI_ACCESS_NONE)
    {
#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
      /* Register the QSPI MSP Callbacks */
      if(QSPICtx[Instance].IsMspCallbacksValid == 0UL)
      {
        if(BSP_QSPI_RegisterDefaultMspCallbacks(Instance) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_PERIPH_FAILURE;
        }
      }
#else
      /* Msp QSPI initialization */
      QSPI_MspInit(&hqspi);
#endif /* USE_HAL_QSPI_REGISTER_CALLBACKS */

      /* Get Flash informations of one memory */
      (void)MT25QL512ABB_GetFlashInfo(&pInfo);

      /* Fill config structure */
      qspi_config.ClockPrescaler = PrescalerTab[Init->TransferRate];
      qspi_config.DualFlashMode  = QSPI_DUALFLASH_ENABLE;
      qspi_config.FlashSize      = (Init->DualFlashMode == BSP_QSPI_DUALFLASH_ENABLE) ? (uint32_t)POSITION_VAL((uint32_t)pInfo.FlashSize) : ((uint32_t)POSITION_VAL((uint32_t)pInfo.FlashSize) - 1U);
      qspi_config.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;

      /* Update QSPI CTx value */
      QSPICtx[Instance].DualFlashMode = Init->DualFlashMode;

      /* STM32 QSPI interface initialization in dual flash mode enabled */
      if(MX_QSPI_Init(&hqspi, &qspi_config) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }/* QSPI memory reset */
      else if(QSPI_ResetMemory(Instance) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        qspi_config.DualFlashMode  = (uint32_t)Init->DualFlashMode;

        /* STM32 QSPI interface initialization in requested dual flash mode */
        if(MX_QSPI_Init(&hqspi, &qspi_config) != HAL_OK)
        {
          ret = BSP_ERROR_PERIPH_FAILURE;
        }
        else if(MT25QL512ABB_AutoPollingMemReady(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }/* Force Flash enter 4 Byte address mode */
        else if(MT25QL512ABB_Enter4BytesAddressMode(&hqspi, QSPICtx[Instance].InterfaceMode) != MT25QL512ABB_OK)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }/* Configuration of the Output driver strength on memory side */
        else if(QSPI_SetODS(Instance) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }/* Configure Flash to desired mode */
        else if(BSP_QSPI_ConfigFlash(Instance, Init->InterfaceMode, Init->TransferRate) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
      }
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  De-Initializes the QSPI interface.
  * @param  Instance   QSPI Instance
  * @retval BSP status
  */
int32_t BSP_QSPI_DeInit(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Disable Memory mapped mode */
    if(QSPICtx[Instance].IsInitialized == QSPI_ACCESS_MMP)
    {
      if(BSP_QSPI_DisableMemoryMappedMode(Instance) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
    }

    /* Set default QSPICtx values */
    QSPICtx[Instance].IsInitialized = QSPI_ACCESS_NONE;
    QSPICtx[Instance].InterfaceMode = BSP_QSPI_SPI_MODE;
    QSPICtx[Instance].TransferRate  = BSP_QSPI_STR_TRANSFER;
    QSPICtx[Instance].DualFlashMode = BSP_QSPI_DUALFLASH_DISABLE;

#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 0)
    QSPI_MspDeInit(&hqspi);
#endif /* (USE_HAL_QSPI_REGISTER_CALLBACKS == 0) */

    /* Call the DeInit function to reset the driver */
    if (HAL_QSPI_DeInit(&hqspi) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Initializes the QSPI interface.
  * @param  hqspi          QSPI handle
  * @param  Config         QSPI config structure
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_QSPI_Init(QSPI_HandleTypeDef *hqspi, MX_QSPI_Config *Config)
{
  /* QSPI initialization */
  hqspi->Instance                = QUADSPI;
  hqspi->Init.ClockPrescaler     = Config->ClockPrescaler;
  hqspi->Init.FifoThreshold      = 1;
  hqspi->Init.SampleShifting     = Config->SampleShifting;
  hqspi->Init.FlashSize          = Config->FlashSize;
  hqspi->Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE;  /* Min 50ns required after non Read command */
  hqspi->Init.ClockMode          = QSPI_CLOCK_MODE_0;
  hqspi->Init.FlashID            = QSPI_FLASH_ID_1;            /* Default value at start, can be changed dynamically using BSP_QSPI_SetFlashID() */
  hqspi->Init.DualFlash          = Config->DualFlashMode;

  return HAL_QSPI_Init(hqspi);
}

#if (USE_HAL_QSPI_REGISTER_CALLBACKS == 1)
/**
  * @brief Default BSP QSPI Msp Callbacks
  * @param Instance      QSPI Instance
  * @retval BSP status
  */
int32_t BSP_QSPI_RegisterDefaultMspCallbacks (uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_QSPI_RegisterCallback(&hqspi, HAL_QSPI_MSPINIT_CB_ID, QSPI_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_QSPI_RegisterCallback(&hqspi, HAL_QSPI_MSPDEINIT_CB_ID, QSPI_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      QSPICtx[Instance].IsMspCallbacksValid = 1U;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief BSP QSPI Msp Callback registering
  * @param Instance     QSPI Instance
  * @param CallBacks    pointer to MspInit/MspDeInit callbacks functions
  * @retval BSP status
  */
int32_t BSP_QSPI_RegisterMspCallbacks (uint32_t Instance, BSP_QSPI_Cb_t *CallBacks)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if(HAL_QSPI_RegisterCallback(&hqspi, HAL_QSPI_MSPINIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_QSPI_RegisterCallback(&hqspi, HAL_QSPI_MSPDEINIT_CB_ID, CallBacks->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      QSPICtx[Instance].IsMspCallbacksValid = 1U;
    }
  }

  /* Return BSP status */
  return ret;
}
#endif /* (USE_HAL_QSPI_REGISTER_CALLBACKS == 1) */

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  Instance  QSPI instance
  * @param  pData     Pointer to data to be read
  * @param  ReadAddr  Read start address
  * @param  Size      Size of data to read
  * @retval BSP status
  */
int32_t BSP_QSPI_Read(uint32_t Instance, uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(QSPICtx[Instance].TransferRate == BSP_QSPI_STR_TRANSFER)
    {
      if(MT25QL512ABB_ReadSTR(&hqspi, QSPICtx[Instance].InterfaceMode, MT25QL512ABB_4BYTES_SIZE, pData, ReadAddr, Size) != MT25QL512ABB_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      if(MT25QL512ABB_ReadDTR(&hqspi, QSPICtx[Instance].InterfaceMode, MT25QL512ABB_4BYTES_SIZE, pData, ReadAddr, Size) != MT25QL512ABB_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  * @param  Instance   QSPI instance
  * @param  pData      Pointer to data to be written
  * @param  WriteAddr  Write start address
  * @param  Size       Size of data to write
  * @retval BSP status
  */
int32_t BSP_QSPI_Write(uint32_t Instance, uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t end_addr, current_size, current_addr;
  uint32_t data_addr;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Calculation of the size between the write address and the end of the page */
    current_size = MT25QL512ABB_PAGE_SIZE - (WriteAddr % MT25QL512ABB_PAGE_SIZE);

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > Size)
    {
      current_size = Size;
    }

    /* Initialize the address variables */
    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;
    data_addr = (uint32_t)pData;

    /* Perform the write page by page */
    do
    {
      /* Check if Flash busy ? */
      if(MT25QL512ABB_AutoPollingMemReady(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }/* Enable write operations */
      else if(MT25QL512ABB_WriteEnable(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }/* Issue page program command */
      else if(MT25QL512ABB_PageProgram(&hqspi, QSPICtx[Instance].InterfaceMode, MT25QL512ABB_4BYTES_SIZE, (uint8_t*)data_addr, current_addr, current_size) != MT25QL512ABB_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }/* Configure automatic polling mode to wait for end of program */
      else if (MT25QL512ABB_AutoPollingMemReady(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        data_addr += current_size;
        current_size = ((current_addr + MT25QL512ABB_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : MT25QL512ABB_PAGE_SIZE;
      }
    } while ((current_addr < end_addr) && (ret == BSP_ERROR_NONE));
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Erases the specified block of the QSPI memory.
  *         MT25QL512ABB support 4K, 32K, 64K size block erase commands.
  * @param  Instance     QSPI instance
  * @param  BlockAddress Block address to erase
  * @param  BlockSize    Erase Block size
  * @retval BSP status
  */
int32_t BSP_QSPI_EraseBlock(uint32_t Instance, uint32_t BlockAddress, BSP_QSPI_Erase_t BlockSize)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check Flash busy ? */
    if(MT25QL512ABB_AutoPollingMemReady(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Enable write operations */
    else if(MT25QL512ABB_WriteEnable(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Issue Block Erase command */
    else if(MT25QL512ABB_BlockErase(&hqspi, QSPICtx[Instance].InterfaceMode, MT25QL512ABB_4BYTES_SIZE, BlockAddress, BlockSize) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Erases the entire QSPI memory.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
int32_t BSP_QSPI_EraseChip(uint32_t Instance)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check Flash busy ? */
    if(MT25QL512ABB_AutoPollingMemReady(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Enable write operations */
    else if (MT25QL512ABB_WriteEnable(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Issue Chip erase command */
    else if(MT25QL512ABB_ChipErase(&hqspi, QSPICtx[Instance].InterfaceMode) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Reads current status of the QSPI memory.
  *         If WIP != 0 then return busy.
  * @param  Instance  QSPI instance
  * @retval QSPI memory status: whether busy or not
  */
int32_t BSP_QSPI_GetStatus(uint32_t Instance)
{
  uint8_t reg[2];
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(MT25QL512ABB_ReadStatusRegister(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode, reg) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Check the value of the register */
    else if ((reg[0] & MT25QL512ABB_SR_WIP) != 0U)
    {
      ret = BSP_ERROR_BUSY;
    }
    else if ((QSPICtx[Instance].DualFlashMode == BSP_QSPI_DUALFLASH_ENABLE) && ((reg[1] & MT25QL512ABB_SR_WIP) != 0U))
    {
      ret = BSP_ERROR_BUSY;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Return the configuration of the QSPI memory.
  * @param  Instance  QSPI instance
  * @param  pInfo     pointer on the configuration structure
  * @retval BSP status
  */
int32_t BSP_QSPI_GetInfo(uint32_t Instance, BSP_QSPI_Info_t *pInfo)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    (void)MT25QL512ABB_GetFlashInfo(pInfo);
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Configure the QSPI in memory-mapped mode
  *         Only 1 Instance can running MMP mode. And it will lock system at this mode.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
int32_t BSP_QSPI_EnableMemoryMappedMode(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(QSPICtx[Instance].TransferRate == BSP_QSPI_STR_TRANSFER)
    {
      if(MT25QL512ABB_EnableMemoryMappedModeSTR(&hqspi, QSPICtx[Instance].InterfaceMode, MT25QL512ABB_4BYTES_SIZE) != MT25QL512ABB_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else /* Update QSPI context if all operations are well done */
      {
        QSPICtx[Instance].IsInitialized = QSPI_ACCESS_MMP;
      }
    }
    else
    {
      if(MT25QL512ABB_EnableMemoryMappedModeDTR(&hqspi, QSPICtx[Instance].InterfaceMode, MT25QL512ABB_4BYTES_SIZE) != MT25QL512ABB_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else /* Update QSPI context if all operations are well done */
      {
        QSPICtx[Instance].IsInitialized = QSPI_ACCESS_MMP;
      }
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Exit form memory-mapped mode
  *         Only 1 Instance can running MMP mode. And it will lock system at this mode.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
int32_t BSP_QSPI_DisableMemoryMappedMode(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if(QSPICtx[Instance].IsInitialized != QSPI_ACCESS_MMP)
    {
      ret = BSP_ERROR_QSPI_MMP_UNLOCK_FAILURE;
    }/* Abort MMP back to indirect mode */
    else if(HAL_QSPI_Abort(&hqspi) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if(HAL_QSPI_Init(&hqspi)!= HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else /* Update QSPI context if all operations are well done */
    {
      QSPICtx[Instance].IsInitialized = QSPI_ACCESS_INDIRECT;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  In dual flash mode disabled, Get flash ID 3 Bytes:
  *         Manufacturer ID, Memory type, Memory density
  *         In dual flash mode enabled, Get 2 set of flash ID 3 bytes:
  *         Manufacturer ID 1, Manufacturer ID 2, Memory type 1, Memory type 2,
  *         Memory density 1, Memory density 2
  * @param  Instance  QSPI instance
  * @param  Id Pointer to flash ID bytes
  * @retval BSP status
  */
int32_t BSP_QSPI_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(MT25QL512ABB_ReadID(&hqspi, QSPICtx[Instance].InterfaceMode, Id, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Set Flash to desired Interface mode. And this instance becomes current instance.
  *         If current instance running at MMP mode then this function doesn't work.
  *         Indirect -> Indirect
  * @param  Instance  QSPI instance
  * @param  Mode      QSPI mode
  * @param  Rate      QSPI transfer rate
  * @retval BSP status
  */
int32_t BSP_QSPI_ConfigFlash(uint32_t Instance, BSP_QSPI_Interface_t Mode, BSP_QSPI_Transfer_t Rate)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check if MMP mode locked ************************************************/
    if(QSPICtx[Instance].IsInitialized == QSPI_ACCESS_MMP)
    {
      ret = BSP_ERROR_QSPI_MMP_LOCK_FAILURE;
    }
    else
    {
      /* Setup Flash interface ***************************************************/
      switch(QSPICtx[Instance].InterfaceMode)
      {
      case BSP_QSPI_QPI_MODE :               /* 4-4-4 commands */
        if(Mode != BSP_QSPI_QPI_MODE)
        {
          /* Exit Quad I/O protocol mode */
          if(MT25QL512ABB_ExitQPIMode(&hqspi) != MT25QL512ABB_OK)
          {
            ret = BSP_ERROR_COMPONENT_FAILURE;
          }
          else if(Mode == BSP_QSPI_DPI_MODE)
          {
            /* Enter Dual I/O protocol mode */
            ret = QSPI_EnterDPIMode(Instance);
          }
          else
          {
            /* Nothing to do */
          }
        }
        break;

      case BSP_QSPI_DPI_MODE :               /* 2-2-2 commands */
        if(Mode != BSP_QSPI_DPI_MODE)
        {
          /* Exit Dual I/O protocol mode */
          if(QSPI_ExitDPIMode(Instance) != BSP_ERROR_NONE)
          {
            ret = BSP_ERROR_COMPONENT_FAILURE;
          }
          else if(Mode == BSP_QSPI_QPI_MODE)
          {
            /* Enter Quad I/O protocol mode */
            if(MT25QL512ABB_EnterQPIMode(&hqspi) != MT25QL512ABB_OK)
            {
              ret = BSP_ERROR_COMPONENT_FAILURE;
            }
          }
          else
          {
            /* Nothing to do */
          }
        }
        break;

      case BSP_QSPI_SPI_MODE :               /* 1-1-1 commands, Power on H/W default setting */
      case BSP_QSPI_SPI_1I2O_MODE :          /* 1-1-2 commands */
      case BSP_QSPI_SPI_2IO_MODE :           /* 1-2-2 commands */
      case BSP_QSPI_SPI_1I4O_MODE :          /* 1-1-4 commands */
      case BSP_QSPI_SPI_4IO_MODE :           /* 1-4-4 commands */
      default :
        if(Mode == BSP_QSPI_QPI_MODE)
        {
          /* Enter Quad I/O protocol mode */
          if(MT25QL512ABB_EnterQPIMode(&hqspi) != MT25QL512ABB_OK)
          {
            ret = BSP_ERROR_COMPONENT_FAILURE;
          }
        }
        else if(Mode == BSP_QSPI_DPI_MODE)
        {
          /* Enter Dual I/O protocol mode */
          ret = QSPI_EnterDPIMode(Instance);
        }
        else
        {
          /* Nothing to do */
        }
        break;
      }

      /* Update QSPI context if all operations are well done */
      if(ret == BSP_ERROR_NONE)
      {
        /* Update current status parameter *****************************************/
        QSPICtx[Instance].IsInitialized = QSPI_ACCESS_INDIRECT;
        QSPICtx[Instance].InterfaceMode = Mode;
        QSPICtx[Instance].TransferRate  = Rate;
      }
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Select Flash memory ID
  *         This function is applicable in single flash mode to select one of the two
  *         flash memory available. It has no effect in dual flash mode.
  * @param  Instance  QSPI instance
  * @param  FlashID   Flash memory ID
  * @retval BSP status
  */
int32_t BSP_QSPI_SelectFlashID(uint32_t Instance, uint32_t FlashID)
{
  int32_t ret;

  /* Check if the instance is supported */
  if(Instance >= QSPI_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  } /* Perform flash ID selection */
  else if(HAL_QSPI_SetFlashID(&hqspi, FlashID) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_QSPI_Private_Functions Private Functions
  * @{
  */

/**
  * @brief QSPI MSP Initialization
  * @param hqspi: QSPI handle
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - NVIC configuration for QSPI interrupt
  * @retval None
  */
static void QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hqspi);

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable the QuadSPI memory interface clock */
  QSPI_CLK_ENABLE();

  /* Reset the QuadSPI memory interface */
  QSPI_FORCE_RESET();
  QSPI_RELEASE_RESET();

  /* Enable GPIO clocks */
  QSPI_CLK_GPIO_CLK_ENABLE();

  QSPI_BK1_CS_GPIO_CLK_ENABLE();
  QSPI_BK1_D0_GPIO_CLK_ENABLE();
  QSPI_BK1_D1_GPIO_CLK_ENABLE();
  QSPI_BK1_D2_GPIO_CLK_ENABLE();
  QSPI_BK1_D3_GPIO_CLK_ENABLE();

  QSPI_BK2_CS_GPIO_CLK_ENABLE();
  QSPI_BK2_D0_GPIO_CLK_ENABLE();
  QSPI_BK2_D1_GPIO_CLK_ENABLE();
  QSPI_BK2_D2_GPIO_CLK_ENABLE();
  QSPI_BK2_D3_GPIO_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* QSPI CLK GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_CLK_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = QSPI_CLK_PIN_AF;
  HAL_GPIO_Init(QSPI_CLK_GPIO_PORT, &gpio_init_structure);

  /* QSPI BK1 CS GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_CS_PIN;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Alternate = QSPI_BK1_CS_PIN_AF;
  HAL_GPIO_Init(QSPI_BK1_CS_GPIO_PORT, &gpio_init_structure);

  /* QSPI BK1 D0 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D0_PIN;
  gpio_init_structure.Alternate = QSPI_BK1_D0_PIN_AF;
  HAL_GPIO_Init(QSPI_BK1_D0_GPIO_PORT, &gpio_init_structure);

  /* QSPI BK1 D1 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D1_PIN;
  gpio_init_structure.Alternate = QSPI_BK1_D1_PIN_AF;
  HAL_GPIO_Init(QSPI_BK1_D1_GPIO_PORT, &gpio_init_structure);

  /* QSPI BK1 D2 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D2_PIN;
  gpio_init_structure.Alternate = QSPI_BK1_D2_PIN_AF;
  HAL_GPIO_Init(QSPI_BK1_D2_GPIO_PORT, &gpio_init_structure);

  /* QSPI BK1 D3 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK1_D3_PIN;
  gpio_init_structure.Alternate = QSPI_BK1_D3_PIN_AF;
  HAL_GPIO_Init(QSPI_BK1_D3_GPIO_PORT, &gpio_init_structure);

  /* QSPI BK2 CS GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK2_CS_PIN;
  gpio_init_structure.Alternate = QSPI_BK2_CS_PIN_AF;
  HAL_GPIO_Init(QSPI_BK2_CS_GPIO_PORT, &gpio_init_structure);

  /* QSPI BK2 D0 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK2_D0_PIN;
  gpio_init_structure.Alternate = QSPI_BK2_D0_PIN_AF;
  HAL_GPIO_Init(QSPI_BK2_D0_GPIO_PORT, &gpio_init_structure);

  /* QSPI BK2 D1 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK2_D1_PIN;
  gpio_init_structure.Alternate = QSPI_BK2_D1_PIN_AF;
  HAL_GPIO_Init(QSPI_BK2_D1_GPIO_PORT, &gpio_init_structure);

  /* QSPI BK2 D2 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK2_D2_PIN;
  gpio_init_structure.Alternate = QSPI_BK2_D2_PIN_AF;
  HAL_GPIO_Init(QSPI_BK2_D2_GPIO_PORT, &gpio_init_structure);

  /* QSPI BK2 D3 GPIO pin configuration  */
  gpio_init_structure.Pin       = QSPI_BK2_D3_PIN;
  gpio_init_structure.Alternate = QSPI_BK2_D3_PIN_AF;
  HAL_GPIO_Init(QSPI_BK2_D3_GPIO_PORT, &gpio_init_structure);
}

/**
  * @brief QSPI MSP De-Initialization
  * @param hqspi: QSPI handle
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO and NVIC configuration to their default state
  * @retval None
  */
static void QSPI_MspDeInit(QSPI_HandleTypeDef *hqspi)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hqspi);

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-Configure QSPI pins */
  HAL_GPIO_DeInit(QSPI_CLK_GPIO_PORT, QSPI_CLK_PIN);
  HAL_GPIO_DeInit(QSPI_BK1_CS_GPIO_PORT, QSPI_BK1_CS_PIN);
  HAL_GPIO_DeInit(QSPI_BK1_D0_GPIO_PORT, QSPI_BK1_D0_PIN);
  HAL_GPIO_DeInit(QSPI_BK1_D1_GPIO_PORT, QSPI_BK1_D1_PIN);
  HAL_GPIO_DeInit(QSPI_BK1_D2_GPIO_PORT, QSPI_BK1_D2_PIN);
  HAL_GPIO_DeInit(QSPI_BK1_D3_GPIO_PORT, QSPI_BK1_D3_PIN);
  HAL_GPIO_DeInit(QSPI_BK2_CS_GPIO_PORT, QSPI_BK2_CS_PIN);
  HAL_GPIO_DeInit(QSPI_BK2_D0_GPIO_PORT, QSPI_BK2_D0_PIN);
  HAL_GPIO_DeInit(QSPI_BK2_D1_GPIO_PORT, QSPI_BK2_D1_PIN);
  HAL_GPIO_DeInit(QSPI_BK2_D2_GPIO_PORT, QSPI_BK2_D2_PIN);
  HAL_GPIO_DeInit(QSPI_BK2_D3_GPIO_PORT, QSPI_BK2_D3_PIN);

  /*##-3- Reset peripherals ##################################################*/
  /* Reset the QuadSPI memory interface */
  QSPI_FORCE_RESET();
  QSPI_RELEASE_RESET();

  /* Disable the QuadSPI memory interface clock */
  QSPI_CLK_DISABLE();
}

/**
  * @brief  This function reset the QSPI Flash memory.
  *         Force QPI+DPI+SPI reset to avoid system come from unknown status.
  *         Flash accept SPI commands after reset.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
static int32_t QSPI_ResetMemory(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Send RESET enable command in QPI mode (QUAD I/Os, 4-0-0) */
  if(MT25QL512ABB_ResetEnable(&hqspi, BSP_QSPI_QPI_MODE) != MT25QL512ABB_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }/* Send RESET memory command in QPI mode (QUAD I/Os, 4-0-0) */
  else if(MT25QL512ABB_ResetMemory(&hqspi, BSP_QSPI_QPI_MODE) != MT25QL512ABB_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }/* Send RESET enable command in DPI mode (2-0-0) */
  else if(MT25QL512ABB_ResetEnable(&hqspi, BSP_QSPI_DPI_MODE) != MT25QL512ABB_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }/* Send RESET memory command in DPI mode (2-0-0) */
  else if(MT25QL512ABB_ResetMemory(&hqspi, BSP_QSPI_DPI_MODE) != MT25QL512ABB_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }/* Send RESET enable command in SPI mode (1-0-0) */
  else if(MT25QL512ABB_ResetEnable(&hqspi, BSP_QSPI_SPI_MODE) != MT25QL512ABB_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }/* Send RESET memory command in SPI mode (1-0-0) */
  else if(MT25QL512ABB_ResetMemory(&hqspi, BSP_QSPI_SPI_MODE) != MT25QL512ABB_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    QSPICtx[Instance].IsInitialized = QSPI_ACCESS_INDIRECT;      /* After reset S/W setting to indirect access   */
    QSPICtx[Instance].InterfaceMode = BSP_QSPI_SPI_MODE;         /* After reset H/W back to SPI mode by default  */
    QSPICtx[Instance].TransferRate  = BSP_QSPI_STR_TRANSFER;     /* After reset S/W setting to STR mode          */

    /* After SWreset CMD, wait in case SWReset occurred during erase operation */
    HAL_Delay(MT25QL512ABB_RESET_MAX_TIME);
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  This function configure the Output Driver Strength on memory side.
  *         ODS bit locate in Enhanced Volatile Configuration Register[2:0]
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
static int32_t QSPI_SetODS(uint32_t Instance)
{
  int32_t ret;
  uint8_t reg[2];  /* reg[0] contains reg value for FLASHID_1,
                      reg[1] contains reg value for FLASHID_2 in case of dual flash mode */

  if(MT25QL512ABB_ReadEnhancedVolCfgRegister(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode, reg) != MT25QL512ABB_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Set Output Strength of the QSPI memory as CONF_QSPI_ODS variable ohms */
    MODIFY_REG(reg[0], MT25QL512ABB_EVCR_ODS, (((uint8_t)CONF_QSPI_ODS) << (POSITION_VAL(MT25QL512ABB_EVCR_ODS) & 0x7U)));
    MODIFY_REG(reg[1], MT25QL512ABB_EVCR_ODS, (((uint8_t)CONF_QSPI_ODS) << (POSITION_VAL(MT25QL512ABB_EVCR_ODS) & 0x7U)));

    /* Enable write operations */
    if (MT25QL512ABB_WriteEnable(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Issue Write command */
    else if(MT25QL512ABB_WriteEnhancedVolCfgRegister(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode, reg) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Configure automatic polling mode to wait for end of program */
    else if (MT25QL512ABB_AutoPollingMemReady(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  This function put QSPI memory in DPI mode (Dual I/O) from SPI mode.
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
static int32_t QSPI_EnterDPIMode(uint32_t Instance)
{
  int32_t ret;
  uint8_t reg[2];  /* reg[0] contains reg value for FLASHID_1,
                      reg[1] contains reg value for FLASHID_2 in case of dual flash mode */

  if(MT25QL512ABB_ReadEnhancedVolCfgRegister(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode, reg) != MT25QL512ABB_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Set Dual I/O protocol */
    CLEAR_BIT(reg[0], MT25QL512ABB_EVCR_DPI);
    CLEAR_BIT(reg[1], MT25QL512ABB_EVCR_DPI);

    /* Enable write operations */
    if (MT25QL512ABB_WriteEnable(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Issue Write command */
    else if(MT25QL512ABB_WriteEnhancedVolCfgRegister(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode, reg) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Configure automatic polling mode to wait for end of program */
    else if (MT25QL512ABB_AutoPollingMemReady(&hqspi, BSP_QSPI_DPI_MODE, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

/* Return BSP status */
  return ret;
}

/**
  * @brief  This function put QSPI memory in SPI mode from DPI mode (Dual I/O).
  * @param  Instance  QSPI instance
  * @retval BSP status
  */
static int32_t QSPI_ExitDPIMode(uint32_t Instance)
{
  int32_t ret;
  uint8_t reg[2];  /* reg[0] contains reg value for FLASHID_1,
                      reg[1] contains reg value for FLASHID_2 in case of dual flash mode */

  if(MT25QL512ABB_ReadEnhancedVolCfgRegister(&hqspi, BSP_QSPI_DPI_MODE, QSPICtx[Instance].DualFlashMode, reg) != MT25QL512ABB_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    /* Disable Dual I/O protocol */
    SET_BIT(reg[0], MT25QL512ABB_EVCR_DPI);
    SET_BIT(reg[1], MT25QL512ABB_EVCR_DPI);

    /* Enable write operations */
    if (MT25QL512ABB_WriteEnable(&hqspi, BSP_QSPI_DPI_MODE, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Issue Write command */
    else if(MT25QL512ABB_WriteEnhancedVolCfgRegister(&hqspi, BSP_QSPI_DPI_MODE, QSPICtx[Instance].DualFlashMode, reg) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Configure automatic polling mode to wait for end of program */
    else if (MT25QL512ABB_AutoPollingMemReady(&hqspi, QSPICtx[Instance].InterfaceMode, QSPICtx[Instance].DualFlashMode) != MT25QL512ABB_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
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

