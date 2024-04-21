/**
  ******************************************************************************
  * @file    stm32g474e_eval_smartcard.c
  * @author  MCD Application Team
  * @brief   This file provides a set of firmware functions to manage SmartCard
  *          available on STM32G474E-EVAL board (MB1397) from
  *          STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
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
#include "stm32g474e_eval_smartcard.h"
#include "stm32g474e_eval_errno.h"
#include "stm32g474e_eval_io.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32G474E-EVAL
  * @{
  */

/** @defgroup STM32G474E-EVAL_SMARTCARD STM32G474E-EVAL SMARTCARD
  * @{
  */

/** @defgroup STM32G474E-EVAL_SMARTCARD_Private_Variables SMARTCARD Private Variables
  * @{
  */

SMARTCARD_HandleTypeDef hsmcd ={0};
SMARTCARD_ATR SMARTCARD_A2R;
uint8_t SMARTCARD_ATR_Table[40] = {0};
const uint8_t ICCID[2] = {0x2F, 0xE2};

static __IO uint8_t SMARTCARDData = 0;
static uint32_t F_Table[16] = {372, 372, 558, 744, 1116, 1488, 1860, 0,
                               0, 512, 768, 1024, 1536, 2048, 0, 0};
static uint32_t D_Table[8] = {0, 1, 2, 4, 8, 16, 32, 64};
const uint8_t ICCID_Expected[ICCID_LENGTH] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
/* APDU Transport Structures */
SMARTCARD_ADPU_Commands SMARTCARD_ADPU;
SMARTCARD_ADPU_Response SMARTCARD_Response;

/****************************/
/* USART3 interrupt numbers */
/****************************/
USART_IRQ_t USART3_IRQ =
{
  .IRQn_USART  = USART3_IRQn,
  .IRQn_DMA_rx = DMA1_Channel1_IRQn,
  .IRQn_DMA_tx = DMA1_Channel2_IRQn
};

USART_TypeDef *       SC_USART = SMARTCARD_USART;  /* as defined in the _eval_config */
USART_IRQ_t *         pSC_USART_IRQ = &USART3_IRQ;
USART_Clock_Source_t  SC_USART_CLOCK_SOURCE = USART_CLOCK_SOURCE_PCLK;

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SMARTCARD_Private_FunctionPrototypes SMARTCARD Private Function Prototypes
  * @{
  */
static void SMARTCARD_Init(void);
static void SMARTCARD_DeInit(void);
static void SMARTCARD_PowerCmd(uint32_t NewState);
static void SMARTCARD_Reset(uint32_t ResetState);
static void SMARTCARD_VoltageConfig(uint32_t SC_Voltage);

static void SMARTCARD_Handler(SMARTCARD_State *SMARTCARDState, SMARTCARD_ADPU_Commands *SMARTCARD_ADPU, SMARTCARD_ADPU_Response *SMARTCARD_Response);
static int32_t SMARTCARD_Config(void);
static void SMARTCARD_PTSConfig(void);
static void SMARTCARD_SendData(SMARTCARD_ADPU_Commands *SMARTCARD_ADPU, SMARTCARD_ADPU_Response *SMARTCARD_ResponseStatus);
static void SMARTCARD_AnswerReq(SMARTCARD_State *SMARTCARD_State, uint8_t *card, uint8_t length);
static uint8_t SMARTCARD_Decode_Answer2reset(uint8_t *card);
static int32_t SMARTCARD_Transmit(uint8_t *pData, uint16_t Size);
static int32_t SMARTCARD_Receive(uint8_t *pData, uint16_t Size);
static int32_t SMARTCARD_Abort(void);
static void SMARTCARD_Error(void);

/**
  * @}
  */

/** @defgroup STM32G474E-EVAL_SMARTCARD_Private_Functions SMARTCARD Private Functions
  * @{
  */

/**
  * @brief  Initializes SMARTCARD HAL.
  * @retval BSP status
  */
int32_t BSP_SMARTCARD_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hsmcd.Instance = SMARTCARD_USART;
  hsmcd.Init.BaudRate = 10752;
  hsmcd.Init.WordLength = SMARTCARD_WORDLENGTH_9B;
  hsmcd.Init.StopBits = SMARTCARD_STOPBITS_1_5;
  hsmcd.Init.Parity = SMARTCARD_PARITY_EVEN;
  hsmcd.Init.Mode = SMARTCARD_MODE_TX_RX;
  hsmcd.Init.CLKPolarity = SMARTCARD_POLARITY_LOW;
  hsmcd.Init.CLKPhase = SMARTCARD_PHASE_1EDGE;
  hsmcd.Init.CLKLastBit = SMARTCARD_LASTBIT_ENABLE;
  hsmcd.Init.Prescaler = SMARTCARD_PRESCALER;
  hsmcd.Init.GuardTime = 0;
  hsmcd.Init.NACKEnable = SMARTCARD_NACK_ENABLE;
  hsmcd.Init.OneBitSampling = SMARTCARD_ONE_BIT_SAMPLE_DISABLE;
  hsmcd.Init.TimeOutEnable = SMARTCARD_TIMEOUT_DISABLE;
  hsmcd.Init.BlockLength = 0;  /* T=1 not applicable */
  hsmcd.Init.AutoRetryCount = 3;
  hsmcd.AdvancedInit.AdvFeatureInit = SMARTCARD_ADVFEATURE_NO_INIT;

  if(HAL_SMARTCARD_GetState(&hsmcd) == HAL_SMARTCARD_STATE_RESET)
  {
#if (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 0)
  /* Init the SMARTCARD */
  if (HAL_SMARTCARD_Init(&hsmcd) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
#else
  if(IsSmartCardMspCbValid == 0)
  {
    BSP_BUS_DefaultMspCallbacks(0);
  }
#endif /* (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 0) */

  }
   /* Set RSTIN HIGH */
  SMARTCARD_Reset(SMARTCARD_ENABLE);
  /* Set RSTIN HIGH */
  SMARTCARD_Reset_ON();

  /* Select 5V */
  SMARTCARD_VoltageConfig(SMARTCARD_VOLTAGE_5V);
  HAL_Delay(1);

  /* Enable CMDVCC */
  SMARTCARD_PowerCmd(SMARTCARD_ENABLE);

  /* Set RSTIN LOW */
  HAL_Delay(1);
  SMARTCARD_Reset(SMARTCARD_DISABLE);

  return ret;
}

/**
  * @brief  DeInitializes SMARTCARD HAL.
  * @retval BSP status
  */
int32_t BSP_SMARTCARD_DeInit(void)
{
  /* Disable CMDVCC */
  SMARTCARD_PowerCmd(SMARTCARD_DISABLE);

#if (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 0)
  HAL_SMARTCARD_MspDeInit(&hsmcd);
#else
  IsSmartCardMspCbValid = 0;
#endif /* (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 0) */

  /* Init the SMARTCARD */
  if (HAL_SMARTCARD_DeInit(&hsmcd) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
  * @brief  detect Smartcard presence.
  * @retval BSP_ERROR_NONE if SmartCard is present
  *         BSP_ERROR_COMPONENT_FAILURE if SmartCard is not present
  */
int32_t BSP_SMARTCARD_IsDetected(void)
{
    if ( BSP_IO_ReadPin(0,SMARTCARD_OFF) == IO_PIN_RESET )
         return BSP_ERROR_COMPONENT_FAILURE ;
    else
         return BSP_ERROR_NONE;
}

/**
  * @brief  Smartcard re-config .
  * @retval BSP_ERROR_NONE if SmartCard is re-configured
  *         BSP_ERROR_COMPONENT_FAILURE if SmartCard is not present
  */
int32_t BSP_SMARTCARD_Config(void)
{
  if (BSP_SMARTCARD_IsDetected() != BSP_ERROR_NONE)
        return BSP_ERROR_COMPONENT_FAILURE ;

  SMARTCARD_Config();
  SMARTCARD_PTSConfig();

  return BSP_ERROR_NONE;
}

/**
  * @brief  sends data to the Smartcard
  * @retval BSP_ERROR_NONE if data sent correctly is present
  *         BSP_ERROR_BUSY if SmartCard is busy
  *         BSP_ERROR_COMPONENT_FAILURE if smartcard is not present
  *         BSP_ERROR_BUS_FAILURE if otherwise
  */
int32_t BSP_SMARTCARD_Send(uint8_t *pData, uint16_t Size)
{
  return BSP_ERROR_FEATURE_NOT_SUPPORTED;
}

/**
  * @brief  receives data from the Smartcard
  * @retval BSP_ERROR_NONE if data sent correctly is present
  *         BSP_ERROR_BUSY if SmartCard is busy
  *         BSP_ERROR_COMPONENT_FAILURE if smartcard is not present
  *         BSP_ERROR_BUS_FAILURE if otherwise
  */
int32_t BSP_SMARTCARD_Recv(uint8_t *pData, uint16_t Size)
{
  return BSP_ERROR_FEATURE_NOT_SUPPORTED;
}

/**
  * @brief  abort operation on Smartcard
  * @retval BSP_ERROR_NONE if data sent correctly is present
  *         BSP_ERROR_BUSY if SmartCard is busy
  *         BSP_ERROR_COMPONENT_FAILURE if smartcard is not present
  *         BSP_ERROR_BUS_FAILURE if otherwise
  */
int32_t BSP_SMARTCARD_Abort(void)
{

  return SMARTCARD_Abort();
}
/**
  * @brief  Init the SMARTCARD MSP
  * @retval None
  */
void HAL_SMARTCARD_MspInit(SMARTCARD_HandleTypeDef *hsmartcard)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef USART_clock_source_config, SMARTCARD_clock_source_config;

    __HAL_RCC_USART3_CLK_ENABLE();

    /* periph clock configuration */
    USART_clock_source_config.PeriphClockSelection = RCC_PERIPHCLK_USART3;
    USART_clock_source_config.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig(&USART_clock_source_config);

    /* set SMARTCARD and clock on */
    SMARTCARD_CLK_GPIO_CLK_ENABLE();
    /* Configure USART CK as alternate function  */
    GPIO_InitStruct.Pin   = SMARTCARD_CLK_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(SMARTCARD_CLK_GPIO_PORT, &GPIO_InitStruct);

    /* By default, turn on */
    HAL_GPIO_WritePin(SMARTCARD_CLK_GPIO_PORT, SMARTCARD_CLK_PIN, GPIO_PIN_SET);

    SMARTCARD_IO_GPIO_CLK_ENABLE();
    /* Configure USART Tx (IO) pin as alternate function open-drain */
    GPIO_InitStruct.Pin   = SMARTCARD_IO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(SMARTCARD_IO_GPIO_PORT, &GPIO_InitStruct);

    /* By default, turn on */
    HAL_GPIO_WritePin(SMARTCARD_IO_GPIO_PORT, SMARTCARD_IO_PIN, GPIO_PIN_SET);

    /* power-on 1V8 and 3/5V supplies, OFF, RST and CMDVCC signals */
#if defined(USE_IOEXPANDER)
  /* On eval board, SmartCard OFF, RST, 1V8, 3/5V, CMDVCC connected to MFX */
  BSP_IO_Init_t io_init_structure;

  /* Initialize the IO functionalities */
  io_init_structure.Pin  = (SMARTCARD_1V8 | SMARTCARD_3_5V | SMARTCARD_OFF |SMARTCARD_CMDVCC  | SMARTCARD_RST);
  io_init_structure.Pull = IO_PULLUP;
  io_init_structure.Mode = IO_MODE_OUTPUT_OD;
  /* Initialize IO */
  if(BSP_IO_Init(0, &io_init_structure) == BSP_ERROR_NONE)
  {
     BSP_IO_WritePin(0,(SMARTCARD_1V8 | SMARTCARD_3_5V | SMARTCARD_OFF |SMARTCARD_CMDVCC  | SMARTCARD_RST), IO_PIN_SET);
  }
#endif  /* USE_IOEXPANDER */

 /* periph clock configuration */
 SMARTCARD_clock_source_config.Usart3ClockSelection = RCC_USART3CLKSOURCE_HSI;
 SMARTCARD_clock_source_config.PeriphClockSelection = RCC_PERIPHCLK_USART3;
 HAL_RCCEx_PeriphCLKConfig(&SMARTCARD_clock_source_config);

}

/**
  * @brief  DeInit the SMARTCARD MSP
  * @retval None
  */
void HAL_SMARTCARD_MspDeInit(SMARTCARD_HandleTypeDef *hsmartcard)
{

  SMARTCARD_CLK_GPIO_CLK_DISABLE();
  SMARTCARD_IO_GPIO_CLK_DISABLE();

 #if defined(USE_IOEXPANDER)
  /* On eval board, SmartCard OFF, RST, 1V8, 3/5V, CMDVCC connected to MFX */
  BSP_IO_Init_t io_init_structure;

     io_init_structure.Pin  = (SMARTCARD_1V8 | SMARTCARD_3_5V | SMARTCARD_OFF |SMARTCARD_CMDVCC  | SMARTCARD_RST);
     io_init_structure.Pull = IO_PULLUP;
     io_init_structure.Mode = IO_MODE_OFF;

      BSP_IO_Init(0, &io_init_structure);
  /* do not DeInitialize MFX as it is used by other parts */
#endif  /* USE_IOEXPANDER */

}


/**
  * @brief  Initializes all peripheral used for Smartcard interface.
  * @param  None
  * @retval None
  */
static void SMARTCARD_Init(void)
{

  /* SC_USART configuration --------------------------------------------------*/
  /* SC_USART configured as follow:
  - Word Length = 9 Bits
  - 1.5 Stop Bit
  - Even parity
  - BaudRate = 10752 baud
  - Hardware flow control disabled (RTS and CTS signals)
  - Tx and Rx enabled
  - USART Clock enabled
  */
  hsmcd.Instance = SMARTCARD_USART;
  hsmcd.Init.BaudRate = 10752;   /* Starting baudrate = 4MHz / 372etu */
  hsmcd.Init.WordLength = SMARTCARD_WORDLENGTH_9B; /* Not configurable: 8 bits plus parity */
  hsmcd.Init.StopBits = SMARTCARD_STOPBITS_1_5;    /* Not configurable: 1.5 stop bits */
  hsmcd.Init.Parity = SMARTCARD_PARITY_EVEN;
  hsmcd.Init.Mode = SMARTCARD_MODE_TX_RX;
  hsmcd.Init.CLKPolarity = SMARTCARD_POLARITY_LOW;
  hsmcd.Init.CLKPhase = SMARTCARD_PHASE_1EDGE;
  hsmcd.Init.CLKLastBit = SMARTCARD_LASTBIT_ENABLE;
  hsmcd.Init.Prescaler = SMARTCARD_PRESCALER;
  hsmcd.Init.GuardTime = 0;
  hsmcd.Init.NACKEnable = SMARTCARD_NACK_ENABLE;
  hsmcd.Init.OneBitSampling = SMARTCARD_ONE_BIT_SAMPLE_DISABLE;
  hsmcd.Init.TimeOutEnable = SMARTCARD_TIMEOUT_DISABLE;
  hsmcd.Init.BlockLength = 0;  /* T=1 not applicable */
  hsmcd.Init.AutoRetryCount = 3;
  hsmcd.AdvancedInit.AdvFeatureInit = SMARTCARD_ADVFEATURE_NO_INIT;

  if(HAL_SMARTCARD_GetState(&hsmcd) == HAL_SMARTCARD_STATE_RESET)
  {
#if (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 0)
    /* Init the SMARTCARD */
    if (HAL_SMARTCARD_Init(&hsmcd) != HAL_OK)
    {
         SMARTCARD_Error();
    }
#else
    if(IsSmartCardMspCbValid == 0)
    {
      BSP_BUS_DefaultMspCallbacks(0);
    }
#endif /* (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 0) */
  }
}

/**
  * @brief  DeInitializes all peripheral used for Smartcard interface.
  * @param  None
  * @retval None
  */
static void SMARTCARD_DeInit(void)
{
  /* Disable CMDVCC */
  SMARTCARD_PowerCmd(SMARTCARD_DISABLE);

  /* Deinitializes the hsmcd */
#if (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 0)
  HAL_SMARTCARD_MspDeInit(&hsmcd);
#else
  IsSmartCardMspCbValid = 0;
#endif /* (USE_HAL_SMARTCARD_REGISTER_CALLBACKS == 0) */

  /* Init the SMARTCARD */
  if (HAL_SMARTCARD_DeInit(&hsmcd) != HAL_OK)
  {
         SMARTCARD_Error();
  }
}

/**
* @brief  infinite loop in case of error.
* @param  None
* @retval None
*/
static void SMARTCARD_Error(void)
{
  while(1);
}

/**
* @brief  Enables or disables the power to the Smartcard.
* @param  NewState: new state of the Smartcard power supply.
*         This parameter can be: SMARTCARD_ENABLE or SMARTCARD_DISABLE.
* @retval None
*/
static void SMARTCARD_PowerCmd(uint32_t NewState)
{
    BSP_IO_WritePin(0,SMARTCARD_CMDVCC, (NewState!= SMARTCARD_DISABLE)?IO_PIN_RESET:IO_PIN_SET);
}

/**
  * @brief  Sets or clears the Smartcard reset pin.
  * @param  ResetState: this parameter specifies the state of the Smartcard
  *         reset pin.
  *                 @arg SMARTCARD_ENABLE: to reset the Smartcard.
  *                 @arg SMARTCARD_DISABLE: to un-reset the Smartcard.
  * @retval None
  */
static void SMARTCARD_Reset(uint32_t ResetState)
{
    /* RST active high */
    BSP_IO_WritePin(0,SMARTCARD_RST, (ResetState == IO_PIN_SET)?IO_PIN_SET:IO_PIN_RESET);
}

/**
  * @brief  Configures the card power voltage.
  * @param  SMARTCARD_Voltage: specifies the card power voltage.
  *         This parameter can be one of the following values:
  *              @arg SMARTCARD_VOLTAGE_5V: 5V cards.
  *              @arg SMARTCARD_VOLTAGE_3V: 3V cards.
  * @retval None
  */
static void SMARTCARD_VoltageConfig(uint32_t SMARTCARD_Voltage)
{
  if(SMARTCARD_Voltage == SMARTCARD_VOLTAGE_5V)
  {
    /* Select Smartcard 5V */
    BSP_IO_WritePin(0,SMARTCARD_3_5V, IO_PIN_SET);
  }
  else
  {
    /* Select Smartcard 3V */
    BSP_IO_WritePin(0,SMARTCARD_3_5V, IO_PIN_RESET);
  }
}

/**
  * @brief  Configures the Smartcard.
  * @retval None.
  */
static int32_t SMARTCARD_Config(void)
{
  uint32_t i=0;

  /* Configure Card slot access */
  SMARTCARD_State SMARTCARDState = SMARTCARD_POWER_OFF;

  /* Wait A2R --------------------------------------------------------------*/
  SMARTCARDState = SMARTCARD_POWER_ON;

  SMARTCARD_ADPU.Header.CLA = 0x00;
  SMARTCARD_ADPU.Header.INS = SMARTCARD_GET_A2R;
  SMARTCARD_ADPU.Header.P1 = 0x00;
  SMARTCARD_ADPU.Header.P2 = 0x00;
  SMARTCARD_ADPU.Body.LC = 0x00;

  /* Power on card */
  while(SMARTCARDState != SMARTCARD_ACTIVE_ON_T0)
  {
    SMARTCARD_Handler(&SMARTCARDState, &SMARTCARD_ADPU, &SMARTCARD_Response);
  }

  /* Wait a little after ATR message reception */
  HAL_Delay(100);

  /* Apply the Procedure Type Selection (PTS) */
  SMARTCARD_PTSConfig();

  /* Select MF -------------------------------------------------------------*/
  SMARTCARD_ADPU.Header.CLA = SMARTCARD_CLA_GSM11;
  SMARTCARD_ADPU.Header.INS = SMARTCARD_SELECT_FILE;
  SMARTCARD_ADPU.Header.P1 = 0x00;
  SMARTCARD_ADPU.Header.P2 = 0x00;
  SMARTCARD_ADPU.Body.LC = 0x02;

  /* Select ICCID ----------------------------------------------------------*/
  {
    /* Send APDU Command for ICCID selection */
    SMARTCARD_ADPU.Header.CLA = SMARTCARD_CLA_GSM11;
    SMARTCARD_ADPU.Header.INS = SMARTCARD_SELECT_FILE;
    SMARTCARD_ADPU.Header.P1 = 0x00;
    SMARTCARD_ADPU.Header.P2 = 0x00;
    SMARTCARD_ADPU.Body.LC = 0x02/*0x22*/;

    for(i = 0; i < SMARTCARD_ADPU.Body.LC; i++)
    {
      SMARTCARD_ADPU.Body.Data[i] = ICCID[i];
    }
    while(i < LC_MAX)
    {
      SMARTCARD_ADPU.Body.Data[i++] = 0;
    }
    SMARTCARD_ADPU.Body.LE = 0;

    SMARTCARD_Handler(&SMARTCARDState, &SMARTCARD_ADPU, &SMARTCARD_Response);
  }

  /* Read Binary in ICCID --------------------------------------------------*/
  if(SMARTCARD_Response.SW1 == SMARTCARD_EF_SELECTED)
  {
    SMARTCARD_ADPU.Header.CLA = SMARTCARD_CLA_GSM11;
    SMARTCARD_ADPU.Header.INS = SMARTCARD_READ_BINARY;
    SMARTCARD_ADPU.Header.P1 = 0x00;
    SMARTCARD_ADPU.Header.P2 = 0x00;
    SMARTCARD_ADPU.Body.LC = 0x00;

    SMARTCARD_ADPU.Body.LE = 10;

    SMARTCARD_Handler(&SMARTCARDState, &SMARTCARD_ADPU, &SMARTCARD_Response);
  }
  else
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  /* Check value of read ICCID */
  for (i=0; i<ICCID_LENGTH; i++)
  {
    if(SMARTCARD_Response.Data[i] != ICCID_Expected[i])
    {
      return BSP_ERROR_PERIPH_FAILURE;
    }
  }

  return BSP_ERROR_NONE;
}

/**
  * @brief  Configures the IO speed (BaudRate) communication.
  * @retval None
  */
void SMARTCARD_PTSConfig(void)
{
  uint32_t workingbaudrate = 0, apbclock = 0;
  uint8_t  PPSConfirmStatus = 1;
  uint8_t  SMARTCARD_PPSS[4] = {0};

  /* Reconfigure the USART Baud Rate -----------------------------------------*/
  apbclock = HAL_RCC_GetPCLK1Freq();
  apbclock /= ((SMARTCARD_USART->GTPR & (uint16_t)0x00FF) * 2);

  if((SMARTCARD_A2R.T0 & (uint8_t)0x10) == 0x10)
  {
    if(SMARTCARD_A2R.T[0] != 0x11)
    {
      /* PPSS identifies the PPS request or responce and is equal to 0xFF */
      SMARTCARD_PPSS[0] = 0xFF;
      /* PPS0 indicates by the bits b5, b6, b7 equal to 1 the presence of the optional
      bytes PPSI1, PPS2, PPS3 respectively */
      SMARTCARD_PPSS[1] = 0x10;   /* only send PPS1 */
      /* PPS1 allows the interface device to propose value of F and D to the card */
      SMARTCARD_PPSS[2] = SMARTCARD_A2R.T[0];
      /* PCK check character */
      SMARTCARD_PPSS[3] = (uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SMARTCARD_A2R.T[0];

      if(SMARTCARD_Transmit((uint8_t *)SMARTCARD_PPSS, 4) != BSP_ERROR_NONE)
      {
         SMARTCARD_Error();
      }
#if 0
      uint8_t locData = 0;
      /* PPSS response = PPSS request = 0xFF*/
      if(SMARTCARD_Receive((uint8_t *)&locData, 1) == BSP_ERROR_NONE)
      {
        if(locData != 0xFF)
        {
          /*PPSS exchange unsuccessful */
          PPSConfirmStatus = 0x00;
        }

        /*PPS0 response */
        if(SMARTCARD_Receive((uint8_t *)&locData, 1) == BSP_ERROR_NONE)
        {
          if(locData != 0x10)
          {
            /*PPS0 exchange unsuccessful */
            PPSConfirmStatus = 0x00;
          }
        }
        else
        {
          Error_Handler();
        }
        /*PPS1 response */
        if(SMARTCARD_Receive((uint8_t *)&locData, 1) == BSP_ERROR_NONE)
        {
          if(locData != SMARTCARD_A2R.T[0])
          {
            /*PPS1 exchange unsuccessful */
            PPSConfirmStatus = 0x00;
          }
        }
        else
        {
          Error_Handler();
        }
        /*PCK response */
        if(SMARTCARD_Receive((uint8_t *)&locData, 1) == BSP_ERROR_NONE)
        {
          if(locData != ((uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SMARTCARD_A2R.T[0]))
          {
            /*PCK exchange unsuccessful */
            PPSConfirmStatus = 0x00;
          }
        }
        else
        {
          Error_Handler();
        }
      }
      else
      {
        /*PPSS exchange unsuccessful */
        PPSConfirmStatus = 0x00;
      }
#else
      if(SMARTCARD_Receive(SMARTCARD_PPSS, 4) == BSP_ERROR_NONE)
      {
        if((SMARTCARD_PPSS[0] != 0xFF) ||
           (SMARTCARD_PPSS[1] != 0x10) ||
           (SMARTCARD_PPSS[2] != SMARTCARD_A2R.T[0]) ||
           (SMARTCARD_PPSS[3] != ((uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SMARTCARD_A2R.T[0])))
        {
          /*PCK exchange unsuccessful */
          PPSConfirmStatus = 0x00;
        }
      }
      else
      {
        /*PPSS exchange unsuccessful */
        PPSConfirmStatus = 0x00;
      }
#endif

      /* PPS exchange successful */
      if(PPSConfirmStatus == 0x01)
      {
        workingbaudrate = apbclock * D_Table[(SMARTCARD_A2R.T[0] & (uint8_t)0x0F)];
        workingbaudrate /= F_Table[((SMARTCARD_A2R.T[0] >> 4) & (uint8_t)0x0F)];

        //hsmcd.Instance = SMARTCARD_USART;
        hsmcd.Init.BaudRate = workingbaudrate;
        //hsmcd.Init.WordLength = SMARTCARD_WORDLENGTH_9B;
        //hsmcd.Init.StopBits = SMARTCARD_STOPBITS_1_5;
        //hsmcd.Init.Parity = SMARTCARD_PARITY_EVEN;
        //hsmcd.Init.Mode = SMARTCARD_MODE_TX_RX;
        //hsmcd.Init.CLKPolarity = SMARTCARD_POLARITY_LOW;
        //hsmcd.Init.CLKPhase = SMARTCARD_PHASE_1EDGE;
        //hsmcd.Init.CLKLastBit = SMARTCARD_LASTBIT_ENABLE;
        //hsmcd.Init.Prescaler = 10;
        //hsmcd.Init.GuardTime = 0;
        //hsmcd.Init.NACKEnable = SMARTCARD_NACK_ENABLE;
        //hsmcd.Init.OneBitSampling = SMARTCARD_ONE_BIT_SAMPLE_ENABLE;
        //hsmcd.Init.TimeOutEnable = SMARTCARD_TIMEOUT_DISABLE;
        //hsmcd.Init.BlockLength = 0;  /* T=1 not applicable */
        //hsmcd.Init.AutoRetryCount = 3;
        //hsmcd.AdvancedInit.AdvFeatureInit = SMARTCARD_ADVFEATURE_NO_INIT;
        if(HAL_SMARTCARD_Init(&hsmcd) != HAL_OK)
        {
          SMARTCARD_Error();
        }
      }
    }
  }
}

/**
  * @brief  Manages the Smartcard transport layer: send APDU commands and receives
  *         the APDU response.
  * @param  SMARTCARD_ADPU: pointer to a SMARTCARD_ADPU_Commands structure which will be initialized.
  * @param  SMARTCARD_ResponseStatus: pointer to a SMARTCARD_ADPU_Response structure which will be initialized.
  * @retval None
  */
static void SMARTCARD_SendData(SMARTCARD_ADPU_Commands *SMARTCARD_ADPU, SMARTCARD_ADPU_Response *SMARTCARD_ResponseStatus)
{
  uint32_t i = 0;
  uint8_t SMARTCARD_Command[5] = {0};
  uint8_t locData = 0;

  /* Reset responce buffer ---------------------------------------------------*/
  for(i = 0; i < LC_MAX; i++)
  {
    SMARTCARD_ResponseStatus->Data[i] = 0;
  }

  SMARTCARD_ResponseStatus->SW1 = 0;
  SMARTCARD_ResponseStatus->SW2 = 0;

  /* Send header -------------------------------------------------------------*/
  SMARTCARD_Command[0] = SMARTCARD_ADPU->Header.CLA;
  SMARTCARD_Command[1] = SMARTCARD_ADPU->Header.INS;
  SMARTCARD_Command[2] = SMARTCARD_ADPU->Header.P1;
  SMARTCARD_Command[3] = SMARTCARD_ADPU->Header.P2;
  if(SMARTCARD_ADPU->Body.LC)
  {
    SMARTCARD_Command[4] = SMARTCARD_ADPU->Body.LC;
  }
  else
  {
    SMARTCARD_Command[4] = SMARTCARD_ADPU->Body.LE;
  }

  if(SMARTCARD_Transmit((uint8_t *)SMARTCARD_Command, 5) != BSP_ERROR_NONE)
  {
    SMARTCARD_Error();
  }

  /* --------------------------------------------------------
    Wait Procedure byte from card:
    1 - ACK
    2 - NULL
    3 - SW1; SW2
   -------------------------------------------------------- */
  if(SMARTCARD_Receive((uint8_t *)&locData, 1) == BSP_ERROR_NONE)
  {
    if(((locData & (uint8_t)0xF0) == 0x60) || ((locData & (uint8_t)0xF0) == 0x90))
    {
      /* SW1 received */
      SMARTCARD_ResponseStatus->SW1 = locData;

      if(SMARTCARD_Receive((uint8_t *)&locData, 1) == BSP_ERROR_NONE)
      {
        /* SW2 received */
        SMARTCARD_ResponseStatus->SW2 = locData;
      }
    }
    else if (((locData & (uint8_t)0xFE) == (((uint8_t)~(SMARTCARD_ADPU->Header.INS)) & \
             (uint8_t)0xFE))||((locData & (uint8_t)0xFE) == (SMARTCARD_ADPU->Header.INS & (uint8_t)0xFE)))
    {
      SMARTCARD_ResponseStatus->Data[0] = locData;/* ACK received */
    }
  }
  else
  {
    SMARTCARD_Error();
  }

  /* If no status bytes received ---------------------------------------------*/
  if(SMARTCARD_ResponseStatus->SW1 == 0x00)
  {
    /* Send body data to SMARTCARD   -----------------------------------------*/
    if (SMARTCARD_ADPU->Body.LC)
    {
      if(SMARTCARD_Transmit((uint8_t *)SMARTCARD_ADPU->Body.Data, SMARTCARD_ADPU->Body.LC) != BSP_ERROR_NONE)
      {
        SMARTCARD_Error();
      }
    }

    /* Or receive body data from SMARTCARD  ----------------------------------*/
    else if (SMARTCARD_ADPU->Body.LE)
    {
      for(i = 0; i < SMARTCARD_ADPU->Body.LE; i++)
      {
        if(SMARTCARD_Receive((uint8_t *)&locData, 1) == BSP_ERROR_NONE)
        {
          SMARTCARD_ResponseStatus->Data[i] = locData;
        }
        else
        {
          SMARTCARD_Error();
        }
      }
    }
    /* Wait SW1 --------------------------------------------------------------*/
    if(SMARTCARD_Receive((uint8_t *)&locData, 1) == BSP_ERROR_NONE)
    {
      SMARTCARD_ResponseStatus->SW1 = locData;
    }
    else
    {
      SMARTCARD_Error();
    }
    /* Wait SW2 --------------------------------------------------------------*/
    if(SMARTCARD_Receive((uint8_t *)&locData, 1) == BSP_ERROR_NONE)
    {
      SMARTCARD_ResponseStatus->SW2 = locData;
    }
    else
    {
      SMARTCARD_Error();
    }
  }
}

/**
  * @brief  Requests the reset answer from card.
  * @param  SMARTCARD_State: pointer to an SC_State enumeration that will contain the Smartcard state.
  * @param  card: pointer to a buffer which will contain the card ATR.
  * @param  length: maximum ATR length
  * @retval None
  */
static void SMARTCARD_AnswerReq(SMARTCARD_State *SMARTCARD_State, uint8_t *card, uint8_t length)
{
  switch(*SMARTCARD_State)
  {
  case SMARTCARD_RESET_LOW:
    /* Check responce with reset low -----------------------------------------*/
    SMARTCARD_Receive(card, length);

    if(card[0] != 0x00)
    {
      (*SMARTCARD_State) = SMARTCARD_ACTIVE;
      //SC_Reset(IO_PIN_SET);
    }
    else
    {
      (*SMARTCARD_State) = SMARTCARD_RESET_HIGH;
    }
    break;

  case SMARTCARD_RESET_HIGH:
    /* Check response with reset high ----------------------------------------*/
    SMARTCARD_Reset(SMARTCARD_ENABLE); /* Reset High */

    SMARTCARD_Receive(card, length);

    if(card[0])
    {
      (*SMARTCARD_State) = SMARTCARD_ACTIVE;
    }
    else
    {
      (*SMARTCARD_State) = SMARTCARD_POWER_OFF;
    }
    break;

  case SMARTCARD_ACTIVE:
    break;

  case SMARTCARD_POWER_OFF:
    /* Close Connection if no answer received --------------------------------*/
    SMARTCARD_Reset(IO_PIN_SET); /* Reset high - a bit is used as level shifter from 3.3 to 5 V */
    SMARTCARD_PowerCmd(SMARTCARD_DISABLE);
    break;

  default:
    (*SMARTCARD_State) = SMARTCARD_RESET_LOW;
    break;
  }
}

/**
  * @brief  Decodes the Answer to reset received from card.
  * @param  card: pointer to the buffer containing the card ATR.
  * @retval None
  */
static uint8_t SMARTCARD_Decode_Answer2reset(uint8_t *card)
{
 uint32_t i = 0, flag = 0, buf = 0, protocol = 0;

  SMARTCARD_A2R.TS = card[0];  /* Initial character */
  SMARTCARD_A2R.T0 = card[1];  /* Format character */

  SMARTCARD_A2R.Hlength = SMARTCARD_A2R.T0 & (uint8_t)0x0F;

  if ((SMARTCARD_A2R.T0 & (uint8_t)0x80) == 0x80)
  {
    flag = 1;
  }

  for (i = 0; i < 4; i++)
  {
    SMARTCARD_A2R.Tlength = SMARTCARD_A2R.Tlength + (((SMARTCARD_A2R.T0 & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1);
  }

  for (i = 0; i < SMARTCARD_A2R.Tlength; i++)
  {
    SMARTCARD_A2R.T[i] = card[i + 2];
  }

  protocol = SMARTCARD_A2R.T[SMARTCARD_A2R.Tlength - 1] & (uint8_t)0x0F;

  while (flag)
  {
    if ((SMARTCARD_A2R.T[SMARTCARD_A2R.Tlength - 1] & (uint8_t)0x80) == 0x80)
    {
      flag = 1;
    }
    else
    {
      flag = 0;
    }

    buf = SMARTCARD_A2R.Tlength;
    SMARTCARD_A2R.Tlength = 0;

    for (i = 0; i < 4; i++)
    {
      SMARTCARD_A2R.Tlength = SMARTCARD_A2R.Tlength + (((SMARTCARD_A2R.T[buf - 1] & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1);
    }

    for (i = 0;i < SMARTCARD_A2R.Tlength; i++)
    {
      SMARTCARD_A2R.T[buf + i] = card[i + 2 + buf];
    }
    SMARTCARD_A2R.Tlength += (uint8_t)buf;
  }

  for (i = 0; i < SMARTCARD_A2R.Hlength; i++)
  {
    SMARTCARD_A2R.H[i] = card[i + 2 + SMARTCARD_A2R.Tlength];
  }

  return (uint8_t)protocol;
}

/**
  * @brief  Handles all Smartcard states and serves to send and receive all
  *         communication data between Smartcard and reader.
  * @param  SMARTCARDState: pointer to an SMARTCARD_State enumeration that will contain the
  *         Smartcard state.
  * @param  SMARTCARD_ADPU: pointer to an SMARTCARD_ADPU_Commands structure that will be initialized.
  * @param  SMARTCARD_Response: pointer to a SMARTCARD_ADPU_Response structure which will be initialized.
  * @retval None
  */
void SMARTCARD_Handler(SMARTCARD_State *SMARTCARDState, SMARTCARD_ADPU_Commands *SMARTCARD_ADPU, SMARTCARD_ADPU_Response *SMARTCARD_Response)
{
    uint32_t i = 0;
  switch(*SMARTCARDState)
  {
  case SMARTCARD_POWER_ON:
    if (SMARTCARD_ADPU->Header.INS == SMARTCARD_GET_A2R)
    {
      /* Smartcard intialization ---------------------------------------------*/
      BSP_SMARTCARD_Init();

      /* Reset Data from SMARTCARD buffer -------------------------------------------*/
      for (i = 0; i < 40; i++)
      {
        SMARTCARD_ATR_Table[i] = 0;
      }

      /* Reset SMARTCARD_A2R Structure ----------------------------------------------*/
      SMARTCARD_A2R.TS = 0;
      SMARTCARD_A2R.T0 = 0;
      for (i = 0; i < SETUP_LENGTH; i++)
      {
        SMARTCARD_A2R.T[i] = 0;
      }
      for (i = 0; i < HIST_LENGTH; i++)
      {
        SMARTCARD_A2R.H[i] = 0;
      }
      SMARTCARD_A2R.Tlength = 0;
      SMARTCARD_A2R.Hlength = 0;

      /* Next State ----------------------------------------------------------*/
      *SMARTCARDState = SMARTCARD_RESET_LOW;
    }
    break;

  case SMARTCARD_RESET_LOW:
    if(SMARTCARD_ADPU->Header.INS == SMARTCARD_GET_A2R)
    {
      /* If card is detected then Power ON, Card Reset and wait for an answer) */
      if (BSP_IO_ReadPin(0,SMARTCARD_OFF) != IO_PIN_RESET)
      {
        while(((*SMARTCARDState) != SMARTCARD_POWER_OFF) && ((*SMARTCARDState) != SMARTCARD_ACTIVE))
        {
          SMARTCARD_AnswerReq(SMARTCARDState, (uint8_t *)&SMARTCARD_ATR_Table, 40); /* Check for answer to reset */
        }
      }
      else
      {
        (*SMARTCARDState) = SMARTCARD_POWER_OFF;
      }
    }
    break;

  case SMARTCARD_ACTIVE:
    if (SMARTCARD_ADPU->Header.INS == SMARTCARD_GET_A2R)
    {
      if(SMARTCARD_Decode_Answer2reset(&SMARTCARD_ATR_Table[0]) == T0_PROTOCOL)
      {
        (*SMARTCARDState) = SMARTCARD_ACTIVE_ON_T0;
      }
      else
      {
        (*SMARTCARDState) = SMARTCARD_POWER_OFF;
      }
    }
    break;

  case SMARTCARD_ACTIVE_ON_T0:
    SMARTCARD_SendData(SMARTCARD_ADPU, SMARTCARD_Response);
    break;

  case SMARTCARD_POWER_OFF:
    /* Disable Smartcard interface */
    /* Disable CMDVCC */
    SMARTCARD_PowerCmd(SMARTCARD_DISABLE);

    /* Deinitializes the hsmcd */
    BSP_SMARTCARD_DeInit();
    break;

  default: (*SMARTCARDState) = SMARTCARD_POWER_OFF;
  }
}


/**
  * @brief  Receive data SMARTCARD HAL.
  * @retval None
  */
int32_t SMARTCARD_Receive(uint8_t *pData, uint16_t Size)
{
 /* receive using polling mode */
 if(HAL_SMARTCARD_GetState(&hsmcd) == HAL_SMARTCARD_STATE_READY)
 {
  if ( HAL_SMARTCARD_Receive(&hsmcd, pData, Size, SMARTCARD_TIMEOUT) == HAL_OK)
    return BSP_ERROR_NONE;
  else
    return BSP_ERROR_PERIPH_FAILURE;
  }
 else
 {
  return BSP_ERROR_BUSY;
 }
}

/**
  * @brief  Transmit data SMARTCARD HAL.
  * @retval None
  */
int32_t SMARTCARD_Transmit(uint8_t *pData, uint16_t Size)
{
 /* transmit using polling mode */
 if(HAL_SMARTCARD_GetState(&hsmcd) == HAL_SMARTCARD_STATE_READY)
 {
  if ( HAL_SMARTCARD_Transmit(&hsmcd, pData, Size, SMARTCARD_TIMEOUT) == HAL_OK)
    return BSP_ERROR_NONE;
  else
    return BSP_ERROR_PERIPH_FAILURE;
  }
 else
 {
  return BSP_ERROR_BUSY;
 }
}

/**
  * @brief  Abort the operation SMARTCARD HAL.
  * @retval None
  */
int32_t SMARTCARD_Abort(void)
{
    if ( HAL_SMARTCARD_Abort(&hsmcd) == HAL_OK)
         return BSP_ERROR_NONE;
    else
         return BSP_ERROR_PERIPH_FAILURE;
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

/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
