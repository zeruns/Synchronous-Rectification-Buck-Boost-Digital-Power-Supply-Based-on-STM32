/**
  ******************************************************************************
  * @file    stm32g474e_eval_sd.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32g474e_eval_sd.c driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32G474E_EVAL_SD_H
#define STM32G474E_EVAL_SD_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_conf.h"
#include "stm32g474e_eval_errno.h"
#include "string.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @addtogroup STM32G474E-EVAL_SD
  * @{
  */

/** @defgroup STM32G474E-EVAL_SD_Exported_Types Exported Types
  * @{
  */
typedef struct
{
  uint32_t Reserved1:2U;               /* Reserved */
  uint32_t DeviceSize:12U;             /* Device Size */
  uint32_t MaxRdCurrentVDDMin:3U;      /* Max. read current @ VDD min */
  uint32_t MaxRdCurrentVDDMax:3U;      /* Max. read current @ VDD max */
  uint32_t MaxWrCurrentVDDMin:3U;      /* Max. write current @ VDD min */
  uint32_t MaxWrCurrentVDDMax:3U;      /* Max. write current @ VDD max */
  uint32_t DeviceSizeMul:3U;           /* Device size multiplier */
} SD_Version_1_t;


typedef struct
{
  uint32_t Reserved1:6U;               /* Reserved */
  uint32_t DeviceSize:22U;             /* Device Size */
  uint32_t Reserved2:1U;               /* Reserved */
} SD_Version_2_t;

/**
  * @brief  Card Specific Data: CSD Register
  */
typedef struct
{
  /* Header part */
  uint8_t  CSDStruct:2;            /* CSD structure */
  uint8_t  Reserved1:6;            /* Reserved */
  uint8_t  TAAC:8;                 /* Data read access-time 1 */
  uint8_t  NSAC:8;                 /* Data read access-time 2 in CLK cycles */
  uint8_t  MaxBusClkFrec:8;        /* Max. bus clock frequency */
  uint16_t CardComdClasses:12;      /* Card command classes */
  uint8_t  RdBlockLen:4;           /* Max. read data block length */
  uint8_t  PartBlockRead:1;        /* Partial blocks for read allowed */
  uint8_t  WrBlockMisalign:1;      /* Write block misalignment */
  uint8_t  RdBlockMisalign:1;      /* Read block misalignment */
  uint8_t  DSRImpl:1;              /* DSR implemented */

  /* v1 or v2 struct */
  union csd_version {
    SD_Version_1_t v1;
    SD_Version_2_t v2;
  } version;

  uint8_t  EraseSingleBlockEnable:1;  /* Erase single block enable */
  uint8_t  EraseSectorSize:7;         /* Erase group size multiplier */
  uint8_t  WrProtectGrSize:7;         /* Write protect group size */
  uint8_t  WrProtectGrEnable:1;       /* Write protect group enable */
  uint8_t  Reserved2:2;               /* Reserved */
  uint8_t  WrSpeedFact:3;             /* Write speed factor */
  uint8_t  MaxWrBlockLen:4;           /* Max. write data block length */
  uint8_t  WriteBlockPartial:1;       /* Partial blocks for write allowed */
  uint8_t  Reserved3:5;               /* Reserved */
  uint8_t  FileFormatGrouop:1;        /* File format group */
  uint8_t  CopyFlag:1;                /* Copy flag (OTP) */
  uint8_t  PermWrProtect:1;           /* Permanent write protection */
  uint8_t  TempWrProtect:1;           /* Temporary write protection */
  uint8_t  FileFormat:2;              /* File Format */
  uint8_t  Reserved4:2;               /* Reserved */
  uint8_t  crc:7;                     /* Reserved */
  uint8_t  Reserved5:1;               /* always 1*/
} SD_CardSpecificData_t;

/**
  * @brief  Card Identification Data: CID Register
  */
typedef struct
{
  __IO uint8_t  ManufacturerID;       /* ManufacturerID */
  __IO uint16_t OEM_AppliID;          /* OEM/Application ID */
  __IO uint32_t ProdName1;            /* Product Name part1 */
  __IO uint8_t  ProdName2;            /* Product Name part2*/
  __IO uint8_t  ProdRev;              /* Product Revision */
  __IO uint32_t ProdSN;               /* Product Serial Number */
  __IO uint8_t  Reserved1;            /* Reserved1 */
  __IO uint16_t ManufactDate;         /* Manufacturing Date */
  __IO uint8_t  CID_CRC;              /* CID CRC */
  __IO uint8_t  Reserved2;            /* always 1 */
} SD_CardIdData_t;

/**
  * @brief SD Card information
  */
typedef struct
{
  SD_CardSpecificData_t Csd;
  SD_CardIdData_t Cid;
  uint32_t CardCapacity;              /*!< Card Capacity in MBytes  */
  uint32_t CardBlockSize;             /*!< Card Block Size */
  uint32_t LogBlockNbr;               /*!< Specifies the Card logical Capacity in blocks   */
  uint32_t LogBlockSize;              /*!< Specifies logical block size in bytes           */
  uint8_t  CCS;                       /*!< Card Capacity Status : 0 for Standard Capacity, 1 for High Capacity */
} SD_CardInfo_t;

#if defined (USE_SPI_DMA)
/**
  * @brief  SD handle Structure definition
  */
typedef struct
{ /* no register structure for G4 */
  uint32_t                    *Instance;         /*!< SD registers base address           */
  HAL_LockTypeDef              Lock;             /*!< SD locking object                   */
  uint32_t                     *pTxBuffPtr;      /*!< Pointer to SD Tx transfer Buffer    */
  uint32_t                     TxXferSize;       /*!< SD Tx Transfer size                 */
  uint32_t                     *pRxBuffPtr;      /*!< Pointer to SD Rx transfer Buffer    */
  uint32_t                     RxXferSize;       /*!< SD Rx Transfer size                 */
  __IO uint32_t                Context;          /*!< SD transfer context                 */
  __IO BSP_SD_StateTypeDef     State;            /*!< SD card State                       */
  __IO uint32_t                ErrorCode;        /*!< SD Card Error codes                 */
  DMA_HandleTypeDef            *hdmarx;          /*!< SD Rx DMA handle parameters         */
  DMA_HandleTypeDef            *hdmatx;          /*!< SD Tx DMA handle parameters         */
  BSP_SD_CardInfoTypeDef       SdCard;           /*!< SD Card information                 */
  uint32_t                     CSD[4];           /*!< SD card specific data table         */
  uint32_t                     CID[4];           /*!< SD card identification number table */
}SD_HandleTypeDef;
#endif /* USE_SPI_DMA */

typedef struct {
  uint8_t r1;
  uint8_t r2;   /* also used for R1B */
  uint8_t r3;
  uint8_t r4;
  uint8_t r5;
} SD_CmdAnswer_typedef;
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SD_Exported_Constants Exported Constants
  * @{
  */
/**
  * @brief  Block Size
  */
#define SD_BLOCKSIZE              512U

/**
  * @brief  SD instance number
  */
#define SD_INSTANCES_NBR            1U

/**
  * @brief  SD transfer state definition
  */
#define SD_TRANSFER_OK              0U
#define SD_TRANSFER_BUSY            1U

/**
  * @brief  SD detection on its memory slot
  */
#define SD_PRESENT                 1UL
#define SD_NOT_PRESENT             0UL
#define SD_CHANGED_PRESENT         2UL
#define SD_CHANGED_NOT_PRESENT     3UL

/**
  * @brief  SD capacity type
  */
#define SD_CARD_SDSC               0UL /* SD Standard Capacity */
#define SD_CARD_SDHC               1UL /* SD High Capacity     */

/**
  * @brief SD Card information structure
  */
#define BSP_SD_CardInfo_t SD_CardInfo_t

/**
  * @brief  SD GPIO pins
  */
#define SD_CS_PIN                      GPIO_PIN_8
#define SD_CS_GPIO_PORT                GPIOF
#define SD_CS_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOF_CLK_ENABLE()
#define SD_CS_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOF_CLK_DISABLE()

#define SD_DETECT_PIN                  IO_PIN_5

#define SD_DETECT_EXTI_IRQn            IOEXPANDER_IRQOUT_EXTI_IRQn
#define SD_EXTI_LINE                   EXTI_LINE_0

#define SD_DetectIRQHandler()             HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0)
/**
  * @}
  */

/** @addtogroup STM32G474E-EVAL_SD_Exported_Variables
  * @{
  */
extern SPI_HandleTypeDef hsd_spi;
extern EXTI_HandleTypeDef hsd_exti;
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SD_Exported_Functions Exported Functions
  * @{
  */
int32_t BSP_SD_Init(uint32_t Instance);
int32_t BSP_SD_DeInit(uint32_t Instance);
int32_t BSP_SD_DetectITConfig(uint32_t Instance);
int32_t BSP_SD_ReadBlocks(uint32_t Instance, uint32_t* pData, uint32_t BlockAddr, uint32_t BlockNbr);
int32_t BSP_SD_WriteBlocks(uint32_t Instance,uint32_t* pData, uint32_t BlockAddr, uint32_t BlockNbr);
int32_t BSP_SD_ReadBlocks_DMA(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr);
int32_t BSP_SD_WriteBlocks_DMA(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr);
int32_t BSP_SD_ReadBlocks_IT(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr);
int32_t BSP_SD_WriteBlocks_IT(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr);
int32_t BSP_SD_Erase(uint32_t Instance, uint32_t Blockindex, uint32_t BlockNbr);
int32_t BSP_SD_GetCardState(uint32_t Instance);
int32_t BSP_SD_GetCardInfo(uint32_t Instance, BSP_SD_CardInfo_t *pCardInfo);
int32_t BSP_SD_IsDetected(uint32_t Instance);

/* These functions can be modified in case the current settings (e.g. DMA stream ot IT)
   need to be changed for specific application needs */
void BSP_SD_AbortCallback(uint32_t Instance);
void BSP_SD_WriteCpltCallback(uint32_t Instance);
void BSP_SD_ReadCpltCallback(uint32_t Instance);
void BSP_SD_IRQHandler(void);
void BSP_SD_DetectCallback(uint32_t Instance, uint32_t Status);

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

#ifdef __cplusplus
}
#endif

#endif /* STM32G474E_EVAL_SD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
