/**
  ******************************************************************************
  * @file    stm32g474e_eval_sd.c
  * @author  MCD Application Team
  * @brief   This file includes the uSD card driver mounted on STM32G474E-EVAL
  *          evaluation boards.
  @verbatim
  How To use this driver:
  -----------------------
   - This driver is used to drive the micro SD external cards mounted on STM32G474E-EVAL
     evaluation board.
   - This driver does not need a specific component driver for the micro SD device
     to be included with.

  Driver description:
  ------------------
  + Initialization steps:
     o Initialize the micro SD card using the BSP_SD_Init() function. This
       function includes the MSP layer hardware resources initialization and the
       SDIO interface configuration to interface with the external micro SD. It
       also includes the micro SD initialization sequence for instance 0.

     o To check the SD card presence you can use the function BSP_SD_IsDetected() which
       returns the detection status for instance 0.

     o If SD presence detection interrupt mode is desired, you must configure the
       SD detection interrupt mode by calling the function BSP_SD_DetectITConfig().
       The interrupt is generated as an external interrupt whenever the micro SD card is
       plugged/unplugged in/from the evaluation board.
       The SD detection is managed by MFX, so the SD detection interrupt has to be
       treated by MFX_IRQOUT gpio pin IRQ handler. BSP_SD_DetectCallback() is called when
       SD is detected.

     o The function BSP_SD_GetCardInfo()are used to get the micro SD card information
       which is stored in the structure "HAL_SD_CardInfoTypedef".

  + Micro SD card operations
     o The micro SD card can be accessed with read/write block(s) operations once
       it is ready for access. The access can be performed whether
       using the polling mode by calling the functions BSP_SD_ReadBlocks()/BSP_SD_WriteBlocks().
       The access can be performed to instance 0.

     o The SD erase block(s) is performed using the functions BSP_SD_Erase() with specifying
       the number of blocks to erase. The erase can be performed to instance 0.

     o The SD read/write/erase operations are refering to a block of 512 bytes 
       and to an index among the total number of blocks (of 512 byes) in the card.
       whatever the card capacity

     o The SD runtime status is returned when calling the function BSP_SD_GetCardState().

  @endverbatim
  *          It implements a high level communication layer for read and write
  *          from/to this memory. The needed STM32G474 hardware resources (SPI and
  *          GPIO) are defined in stm32g474e_eval.h file, and the initialization is
  *          performed in SD_LowLevel_Init() function declared in stm32g474e_eval.c
  *          file.
  *          You can easily tailor this driver to any other development board,
  *          by just adapting the defines for hardware resources and
  *          SD_LowLevel_Init() function.
  *
  *          +-------------------------------------------------------+
  *          |                     Pin assignment                    |
  *          +-------------------------+---------------+-------------+
  *          |  STM32G474x SPI Pins    |     SD        |    Pin      |
  *          +-------------------------+---------------+-------------+
  *          | SD_SPI_CS_PIN           |   ChipSelect  |    1        |
  *          | SD_SPI_MOSI_PIN / MOSI  |   DataIn      |    2        |
  *          |                         |   GND         |    3 (0 V)  |
  *          |                         |   VDD         |    4 (3.3 V)|
  *          | SD_SPI_SCK_PIN / SCLK   |   Clock       |    5        |
  *          |                         |   GND         |    6 (0 V)  |
  *          | SD_SPI_MISO_PIN / MISO  |   DataOut     |    7        |
  *          +-------------------------+---------------+-------------+
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

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. How To use this driver:
--------------------------
   - This driver is used to drive the micro SD external card mounted on STM32G474E-EVAL
     (MB1397) evaluation board.
   - This driver does not need a specific component driver for the micro SD device
     to be included with.

2. Driver description:
---------------------
  + Initialization steps:
     o Initialize the micro SD card using the SD_Init() function.
     o To check the SD card presence you can use the function SD_IsDetected() which
       returns the detection status
     o The function SD_GetCardInfo() is used to get the micro SD card information
       which is stored in the structure "HAL_SD_CardInfoTypedef".

  + Micro SD card operations
     o The micro SD card can be accessed with read/write block(s) operations once
       it is ready for access. The access cand be performed in polling
       mode by calling the functions SD_ReadBlocks()/SD_WriteBlocks()
     o The SD erase block(s) is performed using the function SD_Erase() with specifying
       the number of blocks to erase.
     o The SD runtime status is returned when calling the function SD_GetStatus().

------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "stm32g474e_eval_sd.h"
#include "stm32g474e_eval_bus.h"
#include "stm32g474e_eval_io.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @defgroup STM32G474E-EVAL_SD STM32G474E-EVAL SD
  * @brief      This file includes the SD Card driver of
  *             STM32G474E-EVAL boards.
  * @{
  */

/** @defgroup STM32G474E-EVAL_SD_Private_TypesDefinitions Private Types Definitions
  * @{
  */
/**
  * @brief  SD answer format
  */
typedef enum {
 SD_ANSWER_R1_EXPECTED,
 SD_ANSWER_R1B_EXPECTED,
 SD_ANSWER_R2_EXPECTED,
 SD_ANSWER_R3_EXPECTED,
 SD_ANSWER_R4R5_EXPECTED,
 SD_ANSWER_R7_EXPECTED,
 SD_ANSWER_NOT_EXPECTED,
}SD_Answer_t;

/**
  * @brief  SD responses and error flags
  */
typedef enum
{
/* R1 answer value */
  SD_R1_NO_ERROR            = (0x00),
  SD_R1_IN_IDLE_STATE       = (0x01),
  SD_R1_ERASE_RESET         = (0x02),
  SD_R1_ILLEGAL_COMMAND     = (0x04),
  SD_R1_COM_CRC_ERROR       = (0x08),
  SD_R1_ERASE_SEQUENCE_ERROR= (0x10),
  SD_R1_ADDRESS_ERROR       = (0x20),
  SD_R1_PARAMETER_ERROR     = (0x40),
  SD_R1_RESPONSE_FAILURE    = (0xFF),
  SD_R1_RESPONSE_FORMAT7    = (0xF7),

/* R2 answer value */
  SD_R2_NO_ERROR            = 0x00,
  SD_R2_CARD_LOCKED         = 0x01,
  SD_R2_LOCKUNLOCK_ERROR    = 0x02,
  SD_R2_ERROR               = 0x04,
  SD_R2_CC_ERROR            = 0x08,
  SD_R2_CARD_ECC_FAILED     = 0x10,
  SD_R2_WP_VIOLATION        = 0x20,
  SD_R2_ERASE_PARAM         = 0x40,
  SD_R2_OUTOFRANGE          = 0x80,

/**
  * @brief  Data response error
  */
  SD_DATA_OK                = (0x05),
  SD_DATA_CRC_ERROR         = (0x0B),
  SD_DATA_WRITE_ERROR       = (0x0D),
  SD_DATA_OTHER_ERROR       = (0xFF)
} SD_Error_t;

typedef void (* BSP_EXTI_LineCallback) (void);

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SD_Private_Constants Private Constants
  * @{
  */
#define SD_DUMMY_BYTE            0xFFU
#define SD_CMD_LENGTH               6U
#define SD_MAX_TRY                100U    /* Number of try */

/**
  * @brief  Start Data tokens:
  *         Tokens (necessary because at nop/idle (and CS active) only 0xff is
  *         on the data/command line)
  */
#define SD_TOKEN_START_DATA_SINGLE_BLOCK_READ    0xFEU  /* Data token start byte, Start Single Block Read */
#define SD_TOKEN_START_DATA_MULTIPLE_BLOCK_READ  0xFEU  /* Data token start byte, Start Multiple Block Read */
#define SD_TOKEN_START_DATA_SINGLE_BLOCK_WRITE   0xFEU  /* Data token start byte, Start Single Block Write */
#define SD_TOKEN_START_DATA_MULTIPLE_BLOCK_WRITE 0xFDU  /* Data token start byte, Start Multiple Block Write */
#define SD_TOKEN_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xFDU  /* Data toke stop byte, Stop Multiple Block Write */

/**
  * @brief  Commands: CMDxx = CMD-number | 0x40
  */
#define SD_CMD_GO_IDLE_STATE          0U   /* CMD0 = 0x40  */
#define SD_CMD_SEND_OP_COND           1U   /* CMD1 = 0x41  */
#define SD_CMD_SEND_IF_COND           8U   /* CMD8 = 0x48  */
#define SD_CMD_SEND_CSD               9U   /* CMD9 = 0x49  */
#define SD_CMD_SEND_CID               10U  /* CMD10 = 0x4A */
#define SD_CMD_STOP_TRANSMISSION      12U  /* CMD12 = 0x4C */
#define SD_CMD_SEND_STATUS            13U  /* CMD13 = 0x4D */
#define SD_CMD_SET_BLOCKLEN           16U  /* CMD16 = 0x50 */
#define SD_CMD_READ_SINGLE_BLOCK      17U  /* CMD17 = 0x51 */
#define SD_CMD_READ_MULT_BLOCK        18U  /* CMD18 = 0x52 */
#define SD_CMD_SET_BLOCK_COUNT        23U  /* CMD23 = 0x57 */
#define SD_CMD_WRITE_SINGLE_BLOCK     24U  /* CMD24 = 0x58 */
#define SD_CMD_WRITE_MULT_BLOCK       25U  /* CMD25 = 0x59 */
#define SD_CMD_PROG_CSD               27U  /* CMD27 = 0x5B */
#define SD_CMD_SET_WRITE_PROT         28U  /* CMD28 = 0x5C */
#define SD_CMD_CLR_WRITE_PROT         29U  /* CMD29 = 0x5D */
#define SD_CMD_SEND_WRITE_PROT        30U  /* CMD30 = 0x5E */
#define SD_CMD_SD_ERASE_GRP_START     32U  /* CMD32 = 0x60 */
#define SD_CMD_SD_ERASE_GRP_END       33U  /* CMD33 = 0x61 */
#define SD_CMD_UNTAG_SECTOR           34U  /* CMD34 = 0x62 */
#define SD_CMD_ERASE_GRP_START        35U  /* CMD35 = 0x63 */
#define SD_CMD_ERASE_GRP_END          36U  /* CMD36 = 0x64 */
#define SD_CMD_UNTAG_ERASE_GROUP      37U  /* CMD37 = 0x65 */
#define SD_CMD_ERASE                  38U  /* CMD38 = 0x66 */
#define SD_CMD_SD_APP_OP_COND         41U  /* CMD41 = 0x69 */
#define SD_CMD_APP_CMD                55U  /* CMD55 = 0x77 */
#define SD_CMD_READ_OCR               58U  /* CMD55 = 0x79 */

#define SD_ACMD_SD_STATUS             13  /* ACMD13   */
#define SD_ACMD_SEND_OP_COND          41  /* ACMD41   */
#define SD_ACMD_SEND_SCR              51  /* ACMD51   */

/**
  * @brief  SD card Timeout values
  */
#define SD_WRITE_TIMEOUT         100U
#define SD_READ_TIMEOUT          0xFFFFU
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SD_Private_Macros Private Macros
  * @{
  */
/**
  * @brief  SD chip select unactive (high) : SD not selected.
  * @retval none
  */
#define SD_CS_OFF()    HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_RESET)
#define SD_CS_HIGH()   HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_SET)
/**
  * @brief  SD chip select active(low) : SD selected.
  * @retval none
  */
#define SD_CS_ON()     HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_SET)
#define SD_CS_LOW()    HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_RESET)

#define SD_IO_WriteDummy()   (void)BSP_SPI2_Send((uint8_t*)&DummyByte, 1)

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SD_Private_Variables Private Variables
  * @{
  */
static uint8_t CardType = SD_CARD_SDSC;
static uint32_t DummyByte = (uint32_t)SD_DUMMY_BYTE;

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SD_Exported_Variables Exported Variables
  * @{
  */
EXTI_HandleTypeDef hsd_exti;
/**
  * @}
  */

/** STM32G474E-EVAL_SD_Private_FunctionPrototypes Private Functions
  */
static int32_t SD_WriteBlock(uint32_t Instance, uint32_t* pData, uint32_t WriteAddr, uint16_t BlockSize);
static int32_t SD_ReadBlock(uint32_t Instance, uint32_t* pData, uint32_t ReadAddr, uint16_t BlockSize);

#if defined (USE_SPI_DMA)
static void SD_Msp_Init(SD_HandleTypeDef *hsd);
static void SD_Msp_DeInit(SD_HandleTypeDef *hsd);
#endif /* (USE_SPI_DMA)*/

static int32_t SD_GetCIDRegister(SD_CardIdData_t* Cid);
static int32_t SD_GetCSDRegister(SD_CardSpecificData_t* Csd);
static int32_t SD_GetOCRegister(uint8_t* pCCS);

static uint8_t SD_GetDataResponse(void);
static int32_t SD_GoIdleState(void);

static void SD1_EXTI_Callback(void);

SD_CmdAnswer_typedef SD_SendCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc, uint8_t Answer);

static uint8_t SD_ReadData(void);
static int32_t SD_WaitData(uint8_t data);
static SD_CmdAnswer_typedef SD_WaitResponse( uint8_t Answer, uint8_t Response);

uint8_t SD_IO_ReadByte(void);
uint8_t SD_IO_WriteByte(uint8_t Data);

/* Link functions for SD Card peripheral*/
void SD_IO_Init(void);
void SD_IO_DeInit(void);
void SD_IO_CSState(uint8_t val);
/**
  */

/** @addtogroup STM32G474E-EVAL_SD_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes the SD card device.
  * @param  Instance      SD Instance
  * @retval BSP status
  */
int32_t BSP_SD_Init(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }/* Initialize IO functionalities (MFX) used by SD detect pin */
  else if(BSP_IOEXPANDER_Init(0, IOEXPANDER_IO_MODE)!= BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }
  else
  {
      /* SD pin init (MFX) */
      SD_IO_Init();

      /* Check if SD card is present */
      if(BSP_SD_IsDetected(Instance) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_UNKNOWN_COMPONENT;
      }
      else
      {
          /* SD initialized and set to SPI mode properly */
          ret = SD_GoIdleState();
      }
      /* Interrupt already enabled on BSP_IO_Init, just register callback */
      if (HAL_EXTI_GetHandle(&hsd_exti, SD_EXTI_LINE) == HAL_OK)
      {
       if (HAL_EXTI_RegisterCallback(&hsd_exti, HAL_EXTI_COMMON_CB_ID, SD1_EXTI_Callback) != HAL_OK)
       {
        ret = BSP_ERROR_PERIPH_FAILURE;
       }
      }
      else
      {
       ret = BSP_ERROR_PERIPH_FAILURE;
      }
    }
  return ret;
}

/**
  * @brief  DeInitializes the SD card device.
  * @param  Instance      SD Instance
  * @retval BSP status
  */
int32_t BSP_SD_DeInit(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  SD_CS_OFF();

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
   SD_IO_WriteDummy();
   SD_CS_OFF();
  }
  return ret;
}

/**
  * @brief  Configures Interrupt mode for SD detection pin.
  * @param  Instance      SD Instance
  * @retval BSP status
  */
int32_t BSP_SD_DetectITConfig(uint32_t Instance)
{
  int32_t ret;
  BSP_IO_Init_t io_init_structure;
  UNUSED(Instance);

  io_init_structure.Pin  = SD_DETECT_PIN;
  io_init_structure.Pull = IO_PULLUP;

  /* Configure IO interrupt acquisition mode */
  if(BSP_IO_ReadPin(0, SD_DETECT_PIN) == (int32_t)IO_PIN_RESET)
  {
    io_init_structure.Mode = IO_MODE_IT_RISING_EDGE;
  }
  else
  {
    io_init_structure.Mode = IO_MODE_IT_FALLING_EDGE;
  }

  if(BSP_IO_Init(0, &io_init_structure) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_BUS_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  BSP SD Callback.
  * @param  Instance SD Instance
  * @param  Status   Pin status
  * @retval None.
  */
__weak void BSP_SD_DetectCallback(uint32_t Instance, uint32_t Status)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance); 
  UNUSED(Status);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on JoyPin is triggered. */
}

/**
 * @brief  Detects if SD card is correctly plugged in the memory slot or not.
  * @param Instance  SD Instance
 * @retval Returns BSP_ERROR_COMPONENT_FAILURE if SD is not present
 *                 BSP_ERROR_NONE if SD is present
 *                 BSP_ERROR_WRONG_PARAM if Instance is not correct
 */
int32_t BSP_SD_IsDetected(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if ( BSP_IO_ReadPin(0, SD_DETECT_PIN) == (int32_t)IO_PIN_RESET)
       { ret = BSP_ERROR_NONE; }
    else
       { ret = BSP_ERROR_COMPONENT_FAILURE ;}
  }
  return ret;
}

/**
  * @brief  Erases the specified memory area of the given SD card.
  * @param  Instance    SD Instance
  * @param  Blockindex  Block index from where data is to be erased
  * @param  BlockNbr    Number of SD blocks to erase
  * @retval BSP status
  * @note   BlockNbr and Blockindex are multiple of SD_BLOCKSIZE (512 Bytes)
  */
int32_t BSP_SD_Erase(uint32_t Instance, uint32_t Blockindex, uint32_t BlockNbr)
{
  int32_t ret = BSP_ERROR_PERIPH_FAILURE;
  SD_CmdAnswer_typedef response;
  UNUSED(Instance);

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
   /* Send CMD32 (Erase group start) and check if the SD acknowledged the erase command: R1 response (0x00: no errors) */
   response = SD_SendCmd(SD_CMD_SD_ERASE_GRP_START, Blockindex * ((CardType == SD_CARD_SDHC) ? 1U : SD_BLOCKSIZE), 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED );
   SD_CS_HIGH();
   SD_IO_WriteDummy();
   if (response.r1 == (uint8_t)SD_R1_NO_ERROR)
   {
    /* Send CMD33 (Erase group end) and Check if the SD acknowledged the erase command: R1 response (0x00: no errors) */
    response = SD_SendCmd(SD_CMD_SD_ERASE_GRP_END, ((Blockindex + BlockNbr)) * ((CardType == SD_CARD_SDHC) ? 1U : SD_BLOCKSIZE), 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED );
    SD_CS_HIGH();
    SD_IO_WriteDummy();
    if (response.r1 == (uint8_t)SD_R1_NO_ERROR)
    {
      /* Send CMD38 (Erase) and Check if the SD acknowledged the erase command: R1b response (0x00: no errors) */
      response = SD_SendCmd(SD_CMD_ERASE, 0, 0xFF, (uint8_t)SD_ANSWER_R1B_EXPECTED);

      /* in R1b response, the non-zero value in r2 indicates the cards is no more busy */
      if ( (response.r1 == (uint8_t)SD_R1_NO_ERROR))
      {
              SD_IO_WriteDummy();
              if(response.r2 == (uint8_t)SD_R1_NO_ERROR)
              {
                ret = BSP_ERROR_BUSY;
              }
              else
              {
                ret = BSP_ERROR_NONE;
              }
      }
      SD_CS_HIGH();
      SD_IO_WriteDummy();
    }
   }
  }
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Writes data to a specified address in an SD card, in polling mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the data to transmit
  * @param  BlockAddr  Address of the block from where data is to be written
  * @param  BlockNbr   Number of SD blocks to write
  * @retval BSP status
  * @note   BlockAddr is a multiple of SD_BLOCKSIZE (512 Bytes)
  */
int32_t BSP_SD_WriteBlocks(uint32_t Instance, uint32_t* pData, uint32_t BlockAddr, uint32_t BlockNbr)
{
  int32_t ret = BSP_ERROR_NONE;
  uint8_t *pByte;
  uint32_t block_addr;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    pByte = (uint8_t *)pData;
    block_addr = BlockAddr * SD_BLOCKSIZE;
    /* Data transfer */
    while (BlockNbr--)
    {
       ret = SD_WriteBlock(Instance, (uint32_t *)pByte, block_addr, SD_BLOCKSIZE);
       if (ret != BSP_ERROR_NONE) break;
       /* loop on next block : Set next write address */
       pByte += SD_BLOCKSIZE;
       block_addr += SD_BLOCKSIZE;
    }
  }
  /* Returns the response */
  return ret;
}

/**
  * @brief  Reads data from a specified address in an SD card, in polling mode.
  * @param  Instance        SD Instance
  * @param  pData           Pointer to the buffer that will contain the data to transmit
  * @param  BlockAddr       Address of the block from where data is to be read
  * @param  BlockNbr        Number of SD blocks to read
  * @retval BSP status
  * @note   BlockAddr is a multiple of SD_BLOCKSIZE (512 Bytes)
  */
int32_t BSP_SD_ReadBlocks(uint32_t Instance, uint32_t* pData, uint32_t BlockAddr, uint32_t BlockNbr)
{
  int32_t ret = BSP_ERROR_NONE;
  uint8_t *pByte;
  uint32_t block_addr;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
      pByte = (uint8_t *)pData;
      block_addr = BlockAddr * SD_BLOCKSIZE;
      /* Data transfer */
      while (BlockNbr--)
      {
       ret = SD_ReadBlock(Instance, (uint32_t *)pByte, block_addr, SD_BLOCKSIZE);
       if (ret != BSP_ERROR_NONE) break;
       /* loop on next block : Set next read address */
       pByte += SD_BLOCKSIZE;
       block_addr += SD_BLOCKSIZE;
      }
  }
  /* Returns the response */
  return ret;
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the data to transmit
  * @param  BlockIdx   Block index from where data is to be read
  * @param  BlocksNbr  Number of SD blocks to read
  * @retval BSP status
  */
int32_t BSP_SD_ReadBlocks_DMA(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{
  int32_t ret;
  UNUSED(pData);
  UNUSED(BlockIdx);
  UNUSED(BlocksNbr);

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the data to transmit
  * @param  BlockIdx   Block index from where data is to be written
  * @param  BlocksNbr  Number of SD blocks to write
  * @retval BSP status
  */
int32_t BSP_SD_WriteBlocks_DMA(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{
  int32_t ret;
  UNUSED(pData);
  UNUSED(BlockIdx);
  UNUSED(BlocksNbr);

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the data to transmit
  * @param  BlockIdx   Block index from where data is to be read
  * @param  BlocksNbr  Number of SD blocks to read
  * @retval SD status
  */
int32_t BSP_SD_ReadBlocks_IT(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{
  int32_t ret;
  UNUSED(pData);
  UNUSED(BlockIdx);
  UNUSED(BlocksNbr);

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the data to transmit
  * @param  BlockIdx   Block index from where data is to be written
  * @param  BlocksNbr  Number of SD blocks to write
  * @retval SD status
  */
int32_t BSP_SD_WriteBlocks_IT(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{
  int32_t ret;
  UNUSED(pData);
  UNUSED(BlockIdx);
  UNUSED(BlocksNbr);

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    ret = BSP_ERROR_FEATURE_NOT_SUPPORTED;
  }
  /* Return BSP status */
  return ret;
}

/**
  * @brief  Returns the SD status.
  * @param  Instance   SD Instance
  * @retval The BSP status.
  */
int32_t BSP_SD_GetCardState(uint32_t Instance)
{
  SD_CmdAnswer_typedef response;
  int32_t ret = BSP_ERROR_PERIPH_FAILURE;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
   /* Send CMD13 or ACMD13 and Wait for response in the R2 format (0x00 is no errors) */
   SD_CS_HIGH();
   SD_IO_WriteDummy();
   SD_IO_WriteDummy();
   /* SD chip select low */
   SD_CS_LOW();
   response = SD_SendCmd(SD_CMD_SEND_STATUS, 0, 0xFF, (uint8_t)SD_ANSWER_R2_EXPECTED);

   /* Find SD status according to card state */
   if(( response.r1 == (uint8_t)SD_R1_NO_ERROR) && ( response.r2 == (uint8_t)SD_R2_NO_ERROR))
   {
       ret = BSP_ERROR_NONE;
   }
  }
  /* Send dummy byte: 8 Clock pulses of delay */
  SD_CS_HIGH();
  SD_IO_WriteDummy();

  return ret;
}

/**
  * @brief  Get SD information about specific SD card.
  * @param  Instance  SD Instance
  * @param  pCardInfo Pointer to a BSP_SD_CardInfo structure that contains all SD
  *         card information.
  * @retval BSP Status
  */
int32_t BSP_SD_GetCardInfo(uint32_t Instance, BSP_SD_CardInfo_t *pCardInfo)
{
  int32_t ret;

  if(Instance >= SD_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
     SD_CS_HIGH();
     SD_IO_WriteDummy();

     if (SD_GetCSDRegister(&(pCardInfo->Csd)) != BSP_ERROR_NONE)
     {
       ret = BSP_ERROR_UNKNOWN_FAILURE;
     } 
     else if (SD_GetCIDRegister(&(pCardInfo->Cid)) != BSP_ERROR_NONE) 
     {
       ret = BSP_ERROR_UNKNOWN_FAILURE;
     } 
     else if (SD_GetOCRegister(&(pCardInfo->CCS)) != BSP_ERROR_NONE)
     {
       ret = BSP_ERROR_UNKNOWN_FAILURE;
     } 
     else
     {
       ret = BSP_ERROR_NONE;
     }
     if (ret == BSP_ERROR_NONE)
     {
          /* CardCapacity in MB else it might exceed 32bits */
          /* LogBlockNbr is the max nb of blocks of LogBlockSize (512) Bytes */
          if(CardType == SD_CARD_SDHC )
         {
            pCardInfo->LogBlockSize = 512UL;
            pCardInfo->CardBlockSize = 512UL;
            pCardInfo->CardCapacity = (pCardInfo->Csd.version.v2.DeviceSize + 1U) * 1024UL * pCardInfo->LogBlockSize;
            pCardInfo->LogBlockNbr = (pCardInfo->Csd.version.v2.DeviceSize + 1U) * 1024UL;
          }
          else
          {
            pCardInfo->CardCapacity = ((uint32_t)pCardInfo->Csd.version.v1.DeviceSize + 1U) ;
            pCardInfo->CardCapacity *= (1UL << (pCardInfo->Csd.version.v1.DeviceSizeMul + 2U));
            pCardInfo->LogBlockSize = 512UL;
            pCardInfo->CardBlockSize = (1UL << (uint32_t)pCardInfo->Csd.RdBlockLen);
            pCardInfo->CardCapacity *= pCardInfo->CardBlockSize;
            pCardInfo->LogBlockNbr = (pCardInfo->CardCapacity) / (pCardInfo->LogBlockSize);
          }
     }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief BSP SD Abort callbacks
  * @param  Instance     SD Instance
  * @retval None
  */
__weak void BSP_SD_AbortCallback(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief BSP Tx Transfer completed callbacks
  * @param  Instance     SD Instance
  * @retval None
  */
__weak void BSP_SD_WriteCpltCallback(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief BSP Rx Transfer completed callbacks
  * @param  Instance     SD Instance
  * @retval None
  */
__weak void BSP_SD_ReadCpltCallback(uint32_t Instance)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Instance);
}

/**
  * @brief  This function handles EXTI_LINE_0 interrupt request.
  * @retval None
  */
void BSP_SD_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hsd_exti);
}
/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SD_Private_Functions Private Functions
  * @{
  */
/*******************************************************************************
                            Static Functions
*******************************************************************************/

/**
  * @brief  Writes 1 block to a specified address in an SD card, in polling mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr  address from where data is to be written
  *                    WriteAddr is a multiple of BlockSize
  * @param  BlockSize  Size of SD blocks to write that should be 512
  * @retval BSP status
  */
static int32_t SD_WriteBlock(uint32_t Instance, uint32_t* pData, uint32_t WriteAddr, uint16_t BlockSize)
{
  int32_t ret;
  SD_CmdAnswer_typedef response;

  if((Instance >= SD_INSTANCES_NBR) || ( BlockSize != SD_BLOCKSIZE))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
     uint8_t ptr[SD_BLOCKSIZE];

     SD_CS_LOW();
     SD_IO_WriteDummy();
     SD_IO_WriteDummy();

     /* in case of SDXC/SDHC cards, the BLOCK_LEN is set to 512
         so that CMD16 is not required */
     response = SD_SendCmd(SD_CMD_SET_BLOCKLEN, BlockSize, 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);
     SD_CS_HIGH();
     SD_IO_WriteDummy();
     if ( response.r1 != (uint8_t)SD_R1_NO_ERROR)
     {
          /* Send dummy byte: 8 Clock pulses of delay */
          SD_CS_HIGH();
          SD_IO_WriteDummy();
	      return BSP_ERROR_PERIPH_FAILURE;
     }

     /* Data transfer */
        /* Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write blocks  and
           Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
        response = SD_SendCmd(SD_CMD_WRITE_SINGLE_BLOCK, (WriteAddr * ((CardType == SD_CARD_SDHC) ? 1U : SD_BLOCKSIZE) / SD_BLOCKSIZE), 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);
        if (response.r1 != (uint8_t)SD_R1_NO_ERROR)
        {
          /* Send dummy byte: 8 Clock pulses of delay */
          SD_CS_HIGH();
          SD_IO_WriteDummy();
	      return BSP_ERROR_COMPONENT_FAILURE;
        }

        /* Send dummy byte for NWR timing : one byte between CMDWRITE and TOKEN */
        SD_IO_WriteDummy();
        SD_IO_WriteDummy();

        /* Send the data token to signify the start of the data */
        uint8_t tmp = (uint8_t)SD_TOKEN_START_DATA_SINGLE_BLOCK_WRITE;
        (void)BSP_SPI2_Send(&tmp, 1);

        (void)BSP_SPI2_SendRecv((uint8_t*)pData, ptr, BlockSize);

        /* Put CRC bytes (not really needed by us, but required by SD) */
        SD_IO_WriteDummy();
        SD_IO_WriteDummy();

        /* Read data response for this block write operation */
        if (SD_GetDataResponse() != (uint8_t)SD_DATA_OK)
        {
          /* Send dummy byte: 8 Clock pulses of delay */
          SD_CS_HIGH();
          SD_IO_WriteDummy();
	      return BSP_ERROR_COMPONENT_FAILURE;
        }
        SD_CS_HIGH();
        /* Send a dummy byte */
        SD_IO_WriteDummy();

    ret = BSP_ERROR_NONE;
  }

    /* Send dummy byte: 8 Clock pulses of delay */
    SD_CS_HIGH();
    SD_IO_WriteDummy();
    SD_IO_WriteDummy();
    /* Returns the response */
    return ret;
}

/**
  * @brief  Reads 1 block from a specified address in an SD card, in polling mode.
  * @param  Instance   SD Instance
  * @param  pData      Pointer to the buffer that will contain the readdata
  * @param  ReadAddr   address from where data is to be read
  *                    ReadAddr is a multiple of BlockSize
  * @param  BlockSize  Size of SD blocks to read that should be 512
  * @retval BSP status
  */
static int32_t SD_ReadBlock(uint32_t Instance, uint32_t* pData, uint32_t ReadAddr, uint16_t BlockSize)
{
  int32_t ret;
  SD_CmdAnswer_typedef response;

  if((Instance >= SD_INSTANCES_NBR) || ( BlockSize != SD_BLOCKSIZE))
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    uint8_t ptr[SD_BLOCKSIZE];

      SD_CS_LOW();
      SD_IO_WriteDummy();
      SD_IO_WriteDummy();

      /* in case of SDXC/SDHC cards, the BLOCK_LEN is set to 512
         so that CMD16 is not required */
      /* Send CMD16 (SD_CMD_SET_BLOCKLEN) to set the size of the block and
         Check if the SD acknowledged the set block length command: R1 response (0x00: no errors) */
      response = SD_SendCmd(SD_CMD_SET_BLOCKLEN, BlockSize, 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);
      SD_CS_HIGH();
      SD_IO_WriteDummy();
      if ( response.r1 != (uint8_t)SD_R1_NO_ERROR)
      {
            /* Send dummy byte: 8 Clock pulses of delay */
            SD_CS_HIGH();
            SD_IO_WriteDummy();
	        return BSP_ERROR_COMPONENT_FAILURE;
      }
      /* Data transfer */
       for (uint32_t i = 0 ; i < SD_BLOCKSIZE ; i++)  { ptr[i] = SD_DUMMY_BYTE;}
        /* Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
        /* Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
        response = SD_SendCmd(SD_CMD_READ_SINGLE_BLOCK, (ReadAddr * ((CardType == SD_CARD_SDHC) ? 1U: SD_BLOCKSIZE) / SD_BLOCKSIZE), 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);
        if ( response.r1 != (uint8_t)SD_R1_NO_ERROR)
        {
            /* Send dummy byte: 8 Clock pulses of delay */
            SD_CS_HIGH();
            SD_IO_WriteDummy();
	        return BSP_ERROR_COMPONENT_FAILURE;
        }
        /* Now look for the data token to signify the start of the data */
        if (SD_WaitData(SD_TOKEN_START_DATA_SINGLE_BLOCK_READ) == BSP_ERROR_NONE)
        {
          /* Read the SD block data : read NumByteToRead data */
          if (BSP_SPI2_SendRecv(ptr, (uint8_t*)pData, BlockSize) != BSP_ERROR_NONE)
          {
            /* Send dummy byte: 8 Clock pulses of delay */
            SD_CS_HIGH();
            SD_IO_WriteDummy();
	        return BSP_ERROR_PERIPH_FAILURE;
          }
		  else
          {
            /* Send dummy byte: 8 Clock pulses of delay */
            SD_IO_WriteDummy();
            SD_IO_WriteDummy();
          }
        }
        else
        {
          /* Send dummy byte: 8 Clock pulses of delay */
          SD_CS_HIGH();
          SD_IO_WriteDummy();
	      return BSP_ERROR_COMPONENT_FAILURE;
        }
       /* get CRC bytes (not really needed by us, but required by SD) */
       SD_CS_HIGH();
       SD_IO_WriteDummy();
       /* Set response value to success */
       ret = BSP_ERROR_NONE;
  }
  /* End the command data read cycle */

    /* Send dummy byte: 8 Clock pulses of delay */
    SD_CS_HIGH();
    SD_IO_WriteDummy();

  /* Return the response */
  return ret;
}

/**
  * @brief  SD1 EXTI line detection callbacks.
  * @retval None
  */
static void SD1_EXTI_Callback(void)
{
  int32_t ItStatus;
    /* Read IT status of SD_DETECT */
  ItStatus = BSP_IO_GetIT(0, SD_DETECT_PIN);
  if (ItStatus == (int32_t)IO_PIN_IT_SET)
  {
      if (BSP_SD_IsDetected(0) == BSP_ERROR_NONE)
      {
          BSP_SD_DetectCallback(0, SD_PRESENT);
      }
      else
      {
          BSP_SD_DetectCallback(0, SD_NOT_PRESENT);
      }
      /* change Pin detection orientation */
      (void)BSP_SD_DetectITConfig(0);
      /* Clear IT status of SD_DETECT_PIN */
      ItStatus = BSP_IO_ClearIT(0, SD_DETECT_PIN);
      if (ItStatus < 0)
      {
        /* Nothing to do */
      }
  }
}

/**
  * @brief  Read the CSD card register.
  *         Reading the contents of the CSD register in SPI mode is a simple
  *         read-block transaction.
  * @param  Csd pointer on an SCD register structure
  * @retval BSP status
  */
static int32_t SD_GetCSDRegister(SD_CardSpecificData_t* Csd)
{
  uint8_t counter;
  uint8_t CSD_Tab[16];
  int32_t retr = BSP_ERROR_PERIPH_FAILURE;
  SD_CmdAnswer_typedef response;

  /* SD chip select low */
  SD_CS_LOW();
  /* Send CMD9 (CSD register) and Wait for response in the R1 format (0x00 is no errors) */
  response = SD_SendCmd(SD_CMD_SEND_CSD, 0, 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);

  if(response.r1 == (uint8_t)SD_R1_NO_ERROR)
  {
    if (SD_WaitData(SD_TOKEN_START_DATA_SINGLE_BLOCK_READ) == BSP_ERROR_NONE)
    {
      for (counter = 0; counter < (uint8_t)16; counter++)
      {
        /* Store CSD register value on CSD_Tab */
        CSD_Tab[counter] = SD_IO_ReadByte();
      }

      /* Get CRC bytes (not really needed by us, but required by SD) */
      SD_IO_WriteDummy();
      SD_IO_WriteDummy();

      retr = BSP_ERROR_NONE;

      /*************************************************************************
        CSD header decoding
      *************************************************************************/

      /* Byte 0 */
      Csd->CSDStruct = (CSD_Tab[0] & 0xC0U) >> 6;
      Csd->Reserved1 =  CSD_Tab[0] & 0x3FU;

      /* Byte 1 */
      Csd->TAAC = CSD_Tab[1];

      /* Byte 2 */
      Csd->NSAC = CSD_Tab[2];

      /* Byte 3 */
      Csd->MaxBusClkFrec = CSD_Tab[3];

      /* Byte 4/5 */
      Csd->CardComdClasses = (CSD_Tab[4] << 4) | ((CSD_Tab[5] & 0xF0U) >> 4);
      Csd->RdBlockLen = CSD_Tab[5] & 0x0FU;

      /* Byte 6 */
      Csd->PartBlockRead   = (CSD_Tab[6] & 0x80U) >> 7;
      Csd->WrBlockMisalign = (CSD_Tab[6] & 0x40U) >> 6;
      Csd->RdBlockMisalign = (CSD_Tab[6] & 0x20U) >> 5;
      Csd->DSRImpl         = (CSD_Tab[6] & 0x10U) >> 4;

      /*************************************************************************
        CSD v1/v2 decoding
      *************************************************************************/

      if(CardType == SD_CARD_SDSC)
      {
        Csd->version.v1.Reserved1 = ((CSD_Tab[6] & 0x0CU) >> 2);

        Csd->version.v1.DeviceSize =  ((CSD_Tab[6] & 0x03U) << 10)
                                    |  (CSD_Tab[7] << 2)
                                    | ((CSD_Tab[8] & 0xC0U) >> 6);
        Csd->version.v1.MaxRdCurrentVDDMin = (CSD_Tab[8] & 0x38U) >> 3;
        Csd->version.v1.MaxRdCurrentVDDMax = (CSD_Tab[8] & 0x07U);
        Csd->version.v1.MaxWrCurrentVDDMin = (CSD_Tab[9] & 0xE0U) >> 5;
        Csd->version.v1.MaxWrCurrentVDDMax = (CSD_Tab[9] & 0x1CU) >> 2;
        Csd->version.v1.DeviceSizeMul = ((CSD_Tab[9] & 0x03U) << 1)
                                       |((CSD_Tab[10] & 0x80U) >> 7);
      }
      else 
      {
        Csd->version.v2.Reserved1 = ((CSD_Tab[6] & 0x0FU) << 2) | ((CSD_Tab[7] & 0xC0U) >> 6);
        Csd->version.v2.DeviceSize= ((CSD_Tab[7] & 0x3FU) << 16) | (CSD_Tab[8] << 8) | CSD_Tab[9];
        Csd->version.v2.Reserved2 = ((CSD_Tab[10] & 0x80U) >> 8);
      }

      Csd->EraseSingleBlockEnable = (CSD_Tab[10] & 0x40U) >> 6;
      Csd->EraseSectorSize   = ((CSD_Tab[10] & 0x3FU) << 1)
                              |((CSD_Tab[11] & 0x80U) >> 7);
      Csd->WrProtectGrSize   = (CSD_Tab[11] & 0x7FU);
      Csd->WrProtectGrEnable = (CSD_Tab[12] & 0x80U) >> 7;
      Csd->Reserved2         = (CSD_Tab[12] & 0x60U) >> 5;
      Csd->WrSpeedFact       = (CSD_Tab[12] & 0x1CU) >> 2;
      Csd->MaxWrBlockLen     = ((CSD_Tab[12] & 0x03U) << 2)
                              |((CSD_Tab[13] & 0xC0U) >> 6);
      Csd->WriteBlockPartial = (CSD_Tab[13] & 0x20U) >> 5;
      Csd->Reserved3         = (CSD_Tab[13] & 0x1FU);
      Csd->FileFormatGrouop  = (CSD_Tab[14] & 0x80U) >> 7;
      Csd->CopyFlag          = (CSD_Tab[14] & 0x40U) >> 6;
      Csd->PermWrProtect     = (CSD_Tab[14] & 0x20U) >> 5;
      Csd->TempWrProtect     = (CSD_Tab[14] & 0x10U) >> 4;
      Csd->FileFormat        = (CSD_Tab[14] & 0x0CU) >> 2;
      Csd->Reserved4         = (CSD_Tab[14] & 0x03U);
      Csd->crc               = (CSD_Tab[15] & 0xFEU) >> 1;
      Csd->Reserved5         = (CSD_Tab[15] & 0x01U);
    }
  }

  /* Send dummy byte: 8 Clock pulses of delay */
  SD_CS_HIGH();
  SD_IO_WriteDummy();

  /* Return the reponse */
  return retr;
}

/**
  * @brief  Read the CID card register.
  *         Reading the contents of the CID register in SPI mode is a simple
  *         read-block transaction.
  * @param  Cid pointer on an CID register structure
  * @retval BSP status
  */
static int32_t SD_GetCIDRegister(SD_CardIdData_t* Cid)
{
  uint8_t counter;
  int32_t retr = BSP_ERROR_PERIPH_FAILURE;
  uint8_t CID_Tab[16];

  SD_CmdAnswer_typedef response;

  /* SD chip select low */
  SD_CS_LOW();

  /* Send CMD10 (CID register) and Wait for response in the R1 format (0x00 is no error) */
  response = SD_SendCmd(SD_CMD_SEND_CID, 0, 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);

  if(response.r1 == (uint8_t)SD_R1_NO_ERROR)
  {
    if(SD_WaitData(SD_TOKEN_START_DATA_SINGLE_BLOCK_READ) == BSP_ERROR_NONE)
    {
      /* Store CID register value on CID_Tab */
      for (counter = 0; counter < (uint8_t)16; counter++)
      {
        CID_Tab[counter] = SD_IO_ReadByte();
      }

      /* Get CRC bytes (not really needed by us, but required by SD) */
      SD_IO_WriteDummy();
      SD_IO_WriteDummy();

      /* Byte 0 */
      Cid->ManufacturerID = CID_Tab[0];

      /* Byte 1 */
      Cid->OEM_AppliID = CID_Tab[1] << 8;

      /* Byte 2 */
      Cid->OEM_AppliID |= CID_Tab[2];

      /* Byte 3 */
      Cid->ProdName1 = CID_Tab[3] << 24;

      /* Byte 4 */
      Cid->ProdName1 |= CID_Tab[4] << 16;

      /* Byte 5 */
      Cid->ProdName1 |= CID_Tab[5] << 8;

      /* Byte 6 */
      Cid->ProdName1 |= CID_Tab[6];

      /* Byte 7 */
      Cid->ProdName2 = CID_Tab[7];

      /* Byte 8 */
      Cid->ProdRev = CID_Tab[8];

      /* Byte 9 */
      Cid->ProdSN = CID_Tab[9] << 24;

      /* Byte 10 */
      Cid->ProdSN |= CID_Tab[10] << 16;

      /* Byte 11 */
      Cid->ProdSN |= CID_Tab[11] << 8;

      /* Byte 12 */
      Cid->ProdSN |= CID_Tab[12];

      /* Byte 13 */
      Cid->Reserved1 |= (CID_Tab[13] & 0xF0U) >> 4;
      Cid->ManufactDate = (CID_Tab[13] & 0x0FU) << 8;

      /* Byte 14 */
      Cid->ManufactDate |= CID_Tab[14];

      /* Byte 15 */
      Cid->CID_CRC = (CID_Tab[15] & 0xFEU) >> 1;
      Cid->Reserved2 = 1;

      retr = BSP_ERROR_NONE;
    }
  }

  /* Send dummy byte: 8 Clock pulses of delay */
  SD_CS_HIGH();
  SD_IO_WriteDummy();

  /* Return the reponse */
  return retr;
}

/**
  * @brief  Read the OCR card register.
  *         Reading the contents of the OCR register in SPI mode is a simple
  *         CMD58 transaction.
  * @param  pCCS pointer to the Card Capacity Status
  * @retval BSP status
  */
static int32_t SD_GetOCRegister(uint8_t* pCCS)
{
   int32_t retr = BSP_ERROR_PERIPH_FAILURE;
   SD_CmdAnswer_typedef response;

   SD_CS_LOW();
   /* Send CMD58 (SD_CMD_READ_OCR) to initialize SDHC or SDXC cards: R3 response (0x00: no errors) */
   response = SD_SendCmd(SD_CMD_READ_OCR, 0x0A, 0xFD, (uint8_t)SD_ANSWER_R3_EXPECTED);

   /* CCS is bit 30 of the OCR */

   if(response.r1 == (uint8_t)SD_R1_NO_ERROR)
   {
      /* CCS is 0 for Standard Capacity cards, 1 for High capacity cards */
      *pCCS = ((response.r2 & 0x40U) >> 6);
      retr = BSP_ERROR_NONE;
   }

  /* Send dummy byte: 8 Clock pulses of delay */
  SD_CS_HIGH();
  SD_IO_WriteDummy();

  /* Return the reponse */
  return retr;
}

/**
  * @brief  Send 5 bytes command to the SD card and get response
  * @param  Cmd: The user expected command to send to SD card.
  * @param  Arg: The command argument.
  * @param  Crc: The CRC.
  * @param  Answer: SD_ANSWER_NOT_EXPECTED or SD_ANSWER_EXPECTED
  * @retval SD status
  */
SD_CmdAnswer_typedef SD_SendCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc, uint8_t Answer)
{
  uint8_t frame[SD_CMD_LENGTH], frameout[SD_CMD_LENGTH];
  SD_CmdAnswer_typedef retr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint8_t counter = 0;

  /* R1 Length = NCS(0)+ 6 Bytes command + NCR(min1 max8) + 1 Bytes answer + NEC(0) = 15bytes */
  /* R1b identical to R1 + Busy information                                                   */
  /* R2 Length = NCS(0)+ 6 Bytes command + NCR(min1 max8) + 2 Bytes answer + NEC(0) = 16bytes */

  /* Prepare Frame to send */
  frame[0] = (Cmd | 0x40U);         /* Construct byte 1 */
  frame[1] = (uint8_t)(Arg >> 24U); /* Construct byte 2 */
  frame[2] = (uint8_t)(Arg >> 16U); /* Construct byte 3 */
  frame[3] = (uint8_t)(Arg >> 8U);  /* Construct byte 4 */
  frame[4] = (uint8_t)(Arg);       /* Construct byte 5 */
  frame[5] = (Crc | 0x01U);         /* Construct byte 6 */

  /* Send the command */
  SD_CS_LOW();
  /* Send the Cmd bytes */
  if (BSP_SPI2_SendRecv(frame, frameout, SD_CMD_LENGTH) == BSP_ERROR_NONE) 
  {
    switch(Answer)
    {
    case SD_ANSWER_R1_EXPECTED :
        retr.r1 = SD_ReadData();
        break;
    case SD_ANSWER_R1B_EXPECTED :
        retr.r1 = SD_ReadData();
        retr.r2 = SD_IO_WriteByte(SD_DUMMY_BYTE);
        /* Set CS High */
        SD_CS_HIGH();
        HAL_Delay(1);
        /* Set CS Low */
        SD_CS_LOW();

        /* Wait IO line return 0xFF */      
        while (( retr.r2 == (uint8_t)0) && (counter++  <= SD_MAX_TRY))
        {
          retr.r2 = SD_IO_WriteByte(SD_DUMMY_BYTE);
        }
        break;
    case SD_ANSWER_R2_EXPECTED :
        retr.r1 = SD_ReadData();
        retr.r2 = SD_IO_WriteByte(SD_DUMMY_BYTE);
        break;
    case SD_ANSWER_R3_EXPECTED :
    case SD_ANSWER_R7_EXPECTED :
        retr.r1 = SD_ReadData();
        retr.r2 = SD_IO_WriteByte(SD_DUMMY_BYTE);
        retr.r3 = SD_IO_WriteByte(SD_DUMMY_BYTE);
        retr.r4 = SD_IO_WriteByte(SD_DUMMY_BYTE);
        retr.r5 = SD_IO_WriteByte(SD_DUMMY_BYTE);
        break;
    default :
        break;
    }
  }
    return retr;
}

/**
  * @brief  Get SD card data response.
  * @retval The SD status: Read data response xxx0<status>1
  *         - status 010: Data accepted
  *         - status 101: Data rejected due to a crc error
  *         - status 110: Data rejected due to a Write error.
  *         - status 111: Data rejected due to other error.
  */
uint8_t SD_GetDataResponse(void)
{
  uint8_t dataresponse;
  uint8_t rvalue = (uint8_t)SD_DATA_OTHER_ERROR;

  dataresponse = SD_IO_WriteByte(SD_DUMMY_BYTE);
  (void)SD_IO_WriteByte(SD_DUMMY_BYTE); /* read the busy response byte*/
  /* Mask unused bits */
  switch (dataresponse & (uint8_t)0x1F)
  {
  case SD_DATA_OK:
    rvalue = (uint8_t)SD_DATA_OK;

    /* Set CS High */
    SD_CS_HIGH();
    /* Set CS Low */
    SD_CS_LOW();

    /* Wait IO line return 0xFF */
    while (SD_IO_WriteByte(SD_DUMMY_BYTE) != (uint8_t)0xFF) {};

    break;
  case SD_DATA_CRC_ERROR:
    rvalue =  (uint8_t)SD_DATA_CRC_ERROR;
    break;
  case SD_DATA_WRITE_ERROR:
    rvalue = (uint8_t)SD_DATA_WRITE_ERROR;
    break;
  default:
    break;
  }

  /* Wait null data
  while (SD_IO_ReadByte() != (uint8_t)0);*/
  /* Return response */
  return rvalue;
}

/**
  * @brief  Put SD in Idle state.
  * @retval BSP status
  */
static int32_t SD_GoIdleState(void)
{
  SD_CmdAnswer_typedef response;
  __IO uint8_t counter = 0;
  int32_t status = BSP_ERROR_NONE;

  /* Send CMD0 (SD_CMD_GO_IDLE_STATE) to put SD in SPI mode and
     wait for In Idle State Response (R1 Format) equal to 0x01 */
  /* CS is asserted (low) by SD_SendCmd during the reception of the reset cmd */
  do
  {
    counter++;
    response = SD_SendCmd(SD_CMD_GO_IDLE_STATE, 0, 0x95U, (uint8_t)SD_ANSWER_R1_EXPECTED);
    /* Wait for In Idle State Response (R1 Format) equal to 0x01 */
    SD_IO_CSState(1);
    SD_IO_WriteDummy(); 
    if(counter >= SD_MAX_TRY)
    {
        break;
    }
  }while(response.r1 != (uint8_t)SD_R1_IN_IDLE_STATE);

   HAL_Delay(10); /* insert delay for some slow cards */
   /* try another method for other SD cards */
   if((uint8_t)(response.r1 & 0xFFU) != (uint8_t)SD_R1_IN_IDLE_STATE )
   {
        counter = 0;
        /* Send the command */
        SD_CS_LOW();
        /* wait for more time to be compatible with some (init slow?) card */
        HAL_Delay(1); /* insert delay for some slow cards */
        /* Send CMD0 (SD_CMD_GO_IDLE_STATE) to put SD in SPI mode and
           Wait for In Idle State Response (R1 Format) equal to 0x01 */
        do {
             (void)SD_SendCmd(SD_CMD_GO_IDLE_STATE, 0, 0x95, (uint8_t)SD_ANSWER_NOT_EXPECTED);
             /* wait for more time to be compatible with some (init slow?) card */
             HAL_Delay(1); /* insert delay for some slow cards */
             /* Wait for In Idle State Response (R1 Format) equal to 0x01 */
             response = SD_WaitResponse((uint8_t)SD_ANSWER_R1_EXPECTED, (uint8_t)SD_R1_IN_IDLE_STATE);
             if(counter++ >= SD_MAX_TRY)
             {
               break;
             }
         } while (response.r1 != (uint8_t)SD_R1_IN_IDLE_STATE);
   }

   /* Send CMD8 (SD_CMD_SEND_IF_COND) to check the power supply status
      and wait until response (R7 Format) equal to 0xAA and */
   response = SD_SendCmd(SD_CMD_SEND_IF_COND, 0x1AA, 0x87, (uint8_t)SD_ANSWER_R7_EXPECTED);
   SD_CS_HIGH();
   SD_IO_WriteDummy();
   if((response.r1  & (uint8_t)SD_R1_ILLEGAL_COMMAND) == (uint8_t)SD_R1_ILLEGAL_COMMAND)
   {
        /* initialise card V1 */
        do
        {
            /* initialise card V1 */
            /* Send CMD55 (SD_CMD_APP_CMD) before any ACMD command: R1 response (0x00: no errors) */
            response = SD_SendCmd(SD_CMD_APP_CMD, 0x00000000, 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);
            SD_CS_HIGH();
            SD_IO_WriteDummy();
            if(response.r1 != (uint8_t)SD_R1_IN_IDLE_STATE)
            {
                return BSP_ERROR_COMPONENT_FAILURE;
            }

            /* Send ACMD41 (SD_CMD_SD_APP_OP_COND) to initialize SDHC or SDXC cards: R1 response (0x00: no errors) */
            response = SD_SendCmd(SD_CMD_SD_APP_OP_COND, 0x00000000, 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);
            SD_CS_HIGH();
            SD_IO_WriteDummy();
        }
        while(response.r1 == (uint8_t)SD_R1_IN_IDLE_STATE);
        CardType = (uint8_t)SD_CARD_SDSC;
   }
   else if(response.r1 == (uint8_t)SD_R1_IN_IDLE_STATE)
   {
        /* initialise card V2 */
        do {
            /* Send CMD55 (SD_CMD_APP_CMD) before any ACMD command: R1 response (0x00: no errors) */
            (void)SD_SendCmd(SD_CMD_APP_CMD, 0, 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);
            SD_CS_HIGH();
            SD_IO_WriteDummy();

            /* Send ACMD41 (SD_CMD_SD_APP_OP_COND) to initialize SDHC or SDXC cards: R1 response (0x00: no errors) */
            response = SD_SendCmd(SD_CMD_SD_APP_OP_COND, 0x40000000, 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);
            SD_CS_HIGH();
            SD_IO_WriteDummy();
        }
        while(response.r1 == (uint8_t)SD_R1_IN_IDLE_STATE);

        if((response.r1 & (uint8_t)SD_R1_ILLEGAL_COMMAND) == (uint8_t)SD_R1_ILLEGAL_COMMAND)
        {
            do {
                /* Send CMD55 (SD_CMD_APP_CMD) before any ACMD command: R1 response (0x00: no errors) */

                response = SD_SendCmd(SD_CMD_APP_CMD, 0, 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
                SD_CS_HIGH();
                SD_IO_WriteDummy();
                if(response.r1 != (uint8_t)SD_R1_IN_IDLE_STATE)
                {
                    return BSP_ERROR_COMPONENT_FAILURE;
                }
                /* Send ACMD41 (SD_CMD_SD_APP_OP_COND) to initialize SDHC or SDXC cards: R1 response (0x00: no errors) */
                response = SD_SendCmd(SD_CMD_SD_APP_OP_COND, 0x00U, 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
                SD_CS_HIGH();
                SD_IO_WriteDummy();
            }
            while(response.r1 == (uint8_t)SD_R1_IN_IDLE_STATE);
        }

        /* Send CMD58 (SD_CMD_READ_OCR) to initialize SDHC or SDXC cards: R3 response (0x00: no errors) */
        response = SD_SendCmd(SD_CMD_READ_OCR, 0x00000000, 0xFF, (uint8_t)SD_ANSWER_R3_EXPECTED);
        SD_CS_HIGH();
        SD_IO_WriteDummy();
        if(response.r1 != (uint8_t)SD_R1_NO_ERROR)
        {
            return BSP_ERROR_COMPONENT_FAILURE;
        }
        CardType = (uint8_t)((response.r2 & 0x0040U) >> 6);
   }
   else
   {
        return BSP_ERROR_COMPONENT_FAILURE;
   }

  return status;
}

/**
  * @brief  Waits a data until a value different from SD_DUMMY_BYTE
  * @retval the read value
  */
uint8_t SD_ReadData(void)
{
  uint8_t timeout = (uint8_t)SD_READ_TIMEOUT;
  uint8_t readvalue;

  /* Check if response is got or a timeout is happen */
  do {
    readvalue = SD_IO_WriteByte(SD_DUMMY_BYTE);
    timeout--;
  }while ((readvalue == (uint8_t)SD_DUMMY_BYTE) && (timeout != 0U));

  if (timeout == 0x00U ) 
  {
    readvalue = 0xFFU;
  }

  /* got Right response */
  return readvalue;
}

/**
  * @brief  Waits a data from the SD card
  * @param  Data  Expected data from the SD card
  * @retval BSP status
  */
int32_t SD_WaitData(uint8_t Data)
{
  uint16_t timeout = 0xFFFF;
  uint8_t readvalue, tmp = SD_DUMMY_BYTE;

  /* Check if response is got or a timeout is happen */
  do
  {
    if(BSP_SPI2_SendRecv(&tmp, &readvalue, 1) != BSP_ERROR_NONE)
    {
      return BSP_ERROR_PERIPH_FAILURE;
    }
    timeout--;
  }while ((readvalue != Data) && (timeout != 0U));

  if (timeout == 0U)
  {
    /* After time out */
    return BSP_ERROR_BUSY;
  }

  /* Right response got */
  return BSP_ERROR_NONE;
}

/**
  * @brief  wait for the appropriate response from card
  * @param  Answer format of response. Possible values are :
  *                 SD_ANSWER_R1_EXPECTED
  *                 SD_ANSWER_R1B_EXPECTED
  *                 SD_ANSWER_R2_EXPECTED
  *                 SD_ANSWER_R3_EXPECTED
  *                 SD_ANSWER_R7_EXPECTED
  * @param  Response  he expected Response from the SD card
  * @retval complete response structure
  */
SD_CmdAnswer_typedef SD_WaitResponse(uint8_t Answer, uint8_t Response)
{
  SD_CmdAnswer_typedef response = {0xFF, 0xFF , 0xFF, 0xFF, 0xFF};
  uint32_t timeout = 6;

  switch(Answer)
  {
  case SD_ANSWER_R1_EXPECTED :
    while ((response.r1 != Response) && (timeout != 0U))
    {
      response.r1 = SD_IO_ReadByte();
      timeout--;
    }
    break;
  case SD_ANSWER_R1B_EXPECTED :
    /* R1 format plus the optionnal busy signal */
    while ((response.r1 != Response) && (timeout != 0U))
    {
      response.r1 = SD_IO_ReadByte();
      timeout--;
    }
    timeout = 0xFF;
    /* get the busy signal : poll until a non-zero value is returned */
    do
    {
      response.r2 = SD_IO_ReadByte();
      timeout--;
    } while ((response.r2 == (uint8_t)0) && (timeout != 0U));
    break;
  case SD_ANSWER_R2_EXPECTED :
    while ((response.r1 != Response) && (timeout != 0U))
    {
      response.r1 = SD_IO_ReadByte();
      timeout--;
    }
    response.r2 = SD_IO_ReadByte();
    break;
  case SD_ANSWER_R3_EXPECTED :
  case SD_ANSWER_R7_EXPECTED :
    while ((response.r1 != Response) && (timeout != 0U))
    {
      response.r1 = SD_IO_ReadByte();
      timeout--;
    }
    response.r2 = SD_IO_ReadByte();
    response.r3 = SD_IO_ReadByte();
    response.r4 = SD_IO_ReadByte();
    response.r5 = SD_IO_ReadByte();
    break;
  default :
    break;
  }

  return response;
}
#if defined (USE_SPI_DMA)
/**
  * @brief  Initializes the SD MSP.
  * @param  hsd SD handle
  * @retval None
  */
static void SD_Msp_Init(SD_HandleTypeDef *hsd)
{
  static DMA_HandleTypeDef dma_rx_handle;
  static DMA_HandleTypeDef dma_tx_handle;
  static DMA_HandleTypeDef dma_rx_handle2;
  static DMA_HandleTypeDef dma_tx_handle2;

    /* Enable DMA2 clocks */
    SD_DMAx_TxRx_CLK_ENABLE();

    dma_rx_handle.Init.Channel             = SD_SDMMC_DMAx_Rx_CHANNEL;
    dma_rx_handle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    dma_rx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
    dma_rx_handle.Init.MemInc              = DMA_MINC_ENABLE;
    dma_rx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_rx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    dma_rx_handle.Init.Mode                = DMA_PFCTRL;
    dma_rx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    dma_rx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    dma_rx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    dma_rx_handle.Init.MemBurst            = DMA_MBURST_INC4;
    dma_rx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;
    dma_rx_handle.Instance                 = SD_SDMMC_DMAx_Rx_STREAM;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmarx, dma_rx_handle);

    /* Deinitialize the stream for new transfer */
    (void)HAL_DMA_DeInit(&dma_rx_handle);

    /* Configure the DMA stream */
    (void)HAL_DMA_Init(&dma_rx_handle);

    dma_tx_handle.Init.Channel             = SD_SDMMC_DMAx_Tx_CHANNEL;
    dma_tx_handle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    dma_tx_handle.Init.PeriphInc           = DMA_PINC_DISABLE;
    dma_tx_handle.Init.MemInc              = DMA_MINC_ENABLE;
    dma_tx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_tx_handle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    dma_tx_handle.Init.Mode                = DMA_PFCTRL;
    dma_tx_handle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    dma_tx_handle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    dma_tx_handle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    dma_tx_handle.Init.MemBurst            = DMA_MBURST_INC4;
    dma_tx_handle.Init.PeriphBurst         = DMA_PBURST_INC4;
    dma_tx_handle.Instance                 = SD_SDMMC_DMAx_Tx_STREAM;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmatx, dma_tx_handle);

    /* Deinitialize the stream for new transfer */
    (void)HAL_DMA_DeInit(&dma_tx_handle);

    /* Configure the DMA stream */
    (void)HAL_DMA_Init(&dma_tx_handle);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(SD_SDMMC_DMAx_Rx_IRQn, BSP_SD_RX_IT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(SD_SDMMC_DMAx_Rx_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(SD_SDMMC_DMAx_Tx_IRQn, BSP_SD_TX_IT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(SD_SDMMC_DMAx_Tx_IRQn);

}

/**
  * @brief  DeInitializes the SD MSP.
  * @param  hsd SD handle
  * @retval None
  */
static void SD_Msp_DeInit(SD_HandleTypeDef *hsd)
{
  static DMA_HandleTypeDef dma_rx_handle;
  static DMA_HandleTypeDef dma_tx_handle;
  static DMA_HandleTypeDef dma_rx_handle2;
  static DMA_HandleTypeDef dma_tx_handle2;

    /* Disable NVIC for DMA transfer complete interrupts */
    HAL_NVIC_DisableIRQ(SD_SDMMC_DMAx_Rx_IRQn);
    HAL_NVIC_DisableIRQ(SD_SDMMC_DMAx_Tx_IRQn);

    /* Deinitialize the stream for new transfer */
    dma_rx_handle.Instance = SD_SDMMC_DMAx_Rx_STREAM;
    (void)HAL_DMA_DeInit(&dma_rx_handle);

    /* Deinitialize the stream for new transfer */
    dma_tx_handle.Instance = SD_SDMMC_DMAx_Tx_STREAM;
    (void)HAL_DMA_DeInit(&dma_tx_handle);
}
#endif /* USE_DMA */

/******************************** LINK SD Card ********************************/

/**
  * @brief  DeInitializes the SD Card and put it into StandBy State (Ready for
  *         data transfer).
  * @retval None
  */
void SD_IO_DeInit(void)
{
  BSP_IO_Init_t io_init_structure;

    /* Deinit DIR pin */
    io_init_structure.Pin  = SD_DETECT_PIN ;
    io_init_structure.Pull = IO_PULLDOWN;
    io_init_structure.Mode = IO_MODE_OFF;
   (void)BSP_IO_Init(0, &io_init_structure);

   HAL_NVIC_DisableIRQ(SD_DETECT_EXTI_IRQn);

}

/**
  * @brief  Initializes Detect (MFX IO 5) and CS pins (PF8)
  *         for the SD Card put SD card in SPI mode
  * @retval None
  */
void SD_IO_Init(void)
{
  BSP_IO_Init_t io_init_structure;

  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Configure SD_DETECT_PIN pin: SD Card detect pin  (on MFX) */
  io_init_structure.Pin  = SD_DETECT_PIN ;
  io_init_structure.Pull = IO_PULLUP;
  io_init_structure.Mode = IO_MODE_INPUT;

  (void)BSP_IO_Init(0, &io_init_structure);

  /* Configure SD_CS_PIN pin: SD Card CS pin : output / OpenDrain  */
  SD_CS_GPIO_CLK_ENABLE();
  GPIO_InitStruct.Pin = SD_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  /* SD_CS pin : GPIO F configuration : input */
  HAL_GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStruct);

  /* Enable and set SD EXTI Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(SD_DETECT_EXTI_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(SD_DETECT_EXTI_IRQn);

   /*------------Put SD in SPI mode--------------*/
  /* SD SPI Config */
  (void)BSP_SPI2_Init();
}

/**
  * @brief  set the Chip Select of the SD card according to val
  * @param val  0 or 1
  * @retval None
  */
void SD_IO_CSState(uint8_t val)
{
 if(val == (uint8_t)1U)
 {
    SD_CS_ON();
 }
 else
 {
    SD_CS_OFF();
 }
}

/**
  * @brief  Writes a byte on the SD.
  * @param  Data byte to send.
  * @retval None
  */
uint8_t SD_IO_WriteByte(uint8_t Data)
{
  uint8_t tmp;

  /* Send the byte */
  (void)BSP_SPI2_SendRecv(&Data, &tmp, 1);
  return tmp;
}

/**
  * @brief  Reads a byte on the SD.
  * @retval Data read on the SD card
  */
uint8_t SD_IO_ReadByte(void)
{
  uint8_t readvalue;

  (void)BSP_SPI2_Recv(&readvalue, 1);

  /* Return the shifted data */
  return readvalue;
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
