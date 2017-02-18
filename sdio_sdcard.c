#include "sdio_sdcard.h"
#include "usart1.h"
/*Private Marco------------------------------------------------------------*/
/**
 * @brief SDIO Static flags, TimeOut, FIFO Address
 */
#define NULL 0
#define SDIO_STATIC_FLAGS ((uint32_t)0x000005FF)
#define SDIO_CMD0TIMEOUT  ((uint32_t)0x00010000)

/**
 * @brief Mask for errors Card Status R1 (OCR Register)
 */
#define SD_OCR_ADDR_OUT_OF_RANGE ((uint32_t)0x80000000)
#define SD_OCR_ADDR_MISALIGNED  ((uint32_t)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR  ((uint32_t)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR  ((uint32_t)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM ((uint32_t)0x08000000)
#define SD_OCR_WRITE_PORT_VIOLATION ((uint32_t)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED  ((uint32_t)0x01000000)
#define SD_OCR_COM_CRC_FAILED      ((uint32_t)0x00800000)
#define SD_OCR_ILLEGAL_CMD         ((uint32_t)0x00400000)
#define SD_OCR_CARD_ECC_FAILED     ((uint32_t)0x00200000)
#define SD_OCR_CC_ERROR   ((uint32_t)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR ((uint32_t)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN  ((uint32_t)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN  ((uint32_t)0x00020000)
#define SD_OCR_CID_CSD_OVERWRITE   ((uint32_t)0x00010000)
#define SD_OCR_WP_ERASE_SKIP  ((uint32_t)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED ((uint32_t)0x00004000)
#define SD_OCR_ERASE_RESET  ((uint32_t)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR ((uint32_t)0x00000008)
#define SD_OCR_ERRORBITS  ((uint32_t)0xFDFFE008)

/**
 * @brief Masks for R6 Response
 */
#define SD_R6_GENERAL_UNKNOWN_ERROR  ((uint32_t)0x00002000)
#define SD_R6_ILLEGAL_CMD    ((uint32_t)0x00004000)
#define SD_R6_COM_CRC_FAILED ((uint32_t)0x00008000)

#define SD_VOLTAGE_WINDOW_SD  ((uint32_t)0x80100000)
#define SD_HIGH_CAPACITY  ((uint32_t)0x40000000)
#define SD_STD_CAPACITY  ((uint32_t)0x00000000)
#define SD_CHECK_PATTERN   ((uint32_t)0x000001AA)
#define SD_MAX_VOLT_TRIAL   ((uint32_t)0x0000FFFF)
#define SD_ALLZERO        ((uint32_t))

#define SD_WIDE_BUS_SUPPORT  ((uint32_t)0x00040000)
#define SD_SINGLE_BUS_SUPPORT ((uint32_t)0x00020000)
#define SD_CARD_LOCKED      ((uint32_t)0x02000000)

#define SD_DATATIMEOUT  ((uint32_t)0xFFFFFFFF)
#define SD_0TO7BITS  ((uint32_t)0x000000FF)
#define SD_8TO15BITS ((uint32_t)0x0000FF00)
#define SD_16TO23BITS ((uint32_t)0x00FF0000)
#define SD_24TO32BITS ((uint32_t)0xFF000000)
#define SD_MAX_DATA_LENGTH ((0x01FFFFFF))

#define SD_HALFFIFO ((0X00000008)) ((0X00000008))
#define SD_HALFFIFOBYTES ((uint32_t)0x00000020)


/**
 * @brief Command Class Supported
 */
#define SD_CCCC_LOCK_UNLOCK   ((uint32_t)0x00000080)
#define SD_CCCC_WRITE_PORT   ((uint32_t)0x00000040)
#define SD_CCCC_ERASE     ((uint32_t)0x00000020)

/**
 * @brief Following Commands are SD Card Specific commands.
 *        SDIO_APP_CMD should be sent before sending these commands.
 */
#define SDIO_SEND_IF_COND   ((uint32_t)0x00000008)



/*Private variables-------------------------------------------------------------*/
static uint32_t CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1
static uint32_t CSD_Tab[4], CID_Tab[4], RCA = 0;
static uint8_t  SDSTATUS_Tab[16];
__IO uint32_t StopCondition = 0;
__IO SD_Error TransferError =SD_OK;
__IO uint32_t TransferEnd = 0;
SD_CardInfo SDCardInfo;

/*SDIO Initialization-----------------------------------------------------------*/
SDIO_InitTypeDef SDIO_InitStructure;
SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;

/*Private function prtotypes------------------------------------------------------*/
static SD_Error CmdError(void);
static SD_Error CmdResp1Error(uint8_t cmd);
static SD_Error CmdResp7Error(void);
static SD_Error CmdResp3Error(void);
static SD_Error CmdResp2Error(void);
static SD_Error CmdResp6Error(uint8_t cmd, uint16_t *prca);
static SD_Error SDEnWideBus(FunctionalState NewState);
static SD_Error IsCardProgramming(uint8_t *pstatus);
static SD_Error FindSCR(uint16_t rca, uint32_t *pscr);

uint8_t convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes);

/*Private functions--------------------------------------------------------------------*/

/*
 *Function Name:SD_DeInit
 *Description:Reset SDIO Port
 *Input:null
 *Output:null
 */
void SD_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /*!<Disable SDIO Clock */
  SDIO_ClockCmd(Disable);
  
  /*!<Set Power State to OFF */
  SDIO_SetPowerState(SDIO_PowerState_OFF);
  
  /*!<DeInitializes the SDIO Peripheral */
  SDIO_DeInit();
  
  /*!<Disable the SDIO AHB Clock*/
  RCC_AHBPerpheralClockCmd(RCC_AHBPerph_SDIO,Disable);
  
  /*!< Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /*!< Configure PD.02 CMD line */
  GPIO_Initstructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_Initstructure);
}


/*
 *Function Name:NVIC_Configuration
 *Brief:Configure the SDIO Preemption to the Highest Level
 *Input:null
 *Output:null
 */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_Initstructure;
  
  /*Configure the NVIC Preemption Priority Bits */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_Initstructure.NVIC_IRQChannelCmd = Enable;
  NVIC_Init(&NVIC_InitStructure);
}


/*
 *@brief Returns the DMA End Of Transfer Status.
 *@param None
 *@retval DMA SDIO Channel Status.
 */
uint32_t SD_DMAEndOfTransferStatus(void)
{
  return (uint32_t)DMA_GetFlagStatus(DMA2_FLAG_TC4);   //Channel4 transfer complete flag.
}

/*
 * 函数名：SD_DMA_RxConfig
 * 描述  ：为SDIO接收数据配置DMA2的通道4的请求
 * 输入  ：BufferDST：用于装载数据的变量指针
 		   BufferSize：	缓冲区大小
 * 输出  ：无
 */
void SD_DMA_RxConfigure(uint32_t *BufferDST, uint32_t BufferSize)
{
  DMA_InitTypeDef DMA_InitStructure;
  
  DMA_ClaerFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);
  
  /*!<DMA2 Channel4 disable*/
  DMA_Cmd(DMA2_Channel4,DISABLE); //SDIO为第四通道
  
  /*!< DMA2 Channel4 Config */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BufferDST;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = BufferSize / 4;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priotity_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA2_Channel4, &DMA_Initstructure);
  
  /*!< DMA2_Channel4,enable */
  DMA_CMD(DMA2_Channel4, ENABLE);
}
  
/*
 * 函数名：SD_DMA_TxConfig
 * 描述  ：为SDIO发送数据配置DMA2的通道4的请求
 * 输入  ：BufferDST：用于装载数据的变量指针
 		   BufferSize：	缓冲区大小
 * 输出  ：无
 */
 void SD_DMA_TxConfigure(uint32_t *BufferSRC, uint32_t BufferSize)
{
  DMA_InitTypeDef DMA_InitStructure;
  DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);
   
  /*!<DMA2 Channel4 disable*/
  DMA_Cmd(DMA2_Channel4,DISABLE); //SDIO为第四通道
   
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BufferSRC;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = BufferSize / 4;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priotity_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA2_Channel4, &DMA_Initstructure);
  
  /*!< DMA2_Channel4,enable */
  DMA_CMD(DMA2_Channel4, ENABLE);
 }
 

/*
 * 函数名：GPIO_Configuration
 * 描述  ：初始化SDIO用到的引脚，开启时钟。
 * 输入  ：无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_Initstructure;
  
   /*!< GPIOC and GPIOD Periph clock enable */
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_GPIOC | RCC_AHB2Periph_GPIOD , ENABLE);
  
   /*!< GPIOC and GPIOD Periph clock enable */
  GPIO_Initstructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /*!< Configure PD.02 CMD line */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  
  /*!< Enable the SDIO AHB Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO, ENABLE);
  
  /*!< Enable the DMA2 Clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, Enable);
}



/*
 * 函数名：SD_Init
 * 描述  ：初始化SD卡，使卡处于就绪状态(准备传输数据)
 * 输入  ：无
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：外部调用
 */

 SD_Error SD_Init(void)
 {
   SD_Error errorstatus = SD_OK;
   GPIO_Configuration();
   SDIO_DeInit();
   errorstatus = SD_PowerOn();
   
   if(errorstatus != SD_OK)
   {
     return(errorstatus);
   }
   
   errorstatus = SD_InitializeCards();
   
   if(errorstatus != SD_OK)
   {
     return(errorstatus);
   }
   
   
   /*!< Configure the SDIO peripheral 
  上电识别，卡初始化都完成后，进入数据传输模式，提高读写速度
  速度若超过24M要进入bypass模式	  
  !< on STM32F2xx devices, SDIOCLK is fixed to 48MHz
  !< SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_TRANSFER_CLK_DIV) */ 
   SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV;
   SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
   SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
   SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
   SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
   SDIO_InitStructure.SDIO_HardWareFlowControl = SDIO_HardWareFlowControl_Disable;
   SDIO_Init(&SDIO_InitStructure);
   
   if (errorstatus == SD_OK)
   {
     errorstatus = SD_GetCardInfo(&SDCardInfo);
   }
   
   if (errorstatus == SD_OK)
   {
     errorstatus = SD_SelectDeselect((uint32_t)) (SDCardInfo.RCA << 16);
   }
   
   if (errorstatus == SD_OK)
   {
     errorstatus = SD_EnableWideBusOperation(SDIO_BusWide_4b);
   }
   
   return(errorstatus);
 }


/**
  * @brief  Gets the cuurent sd card data transfer status.
  * @param  None
  * @retval SDTransferState: Data Transfer state.
  *   This value can be: 
  *        - SD_TRANSFER_OK: No data transfer is acting
  *        - SD_TRANSFER_BUSY: Data transfer is acting
  */
SDTransferState SD_GetStatus(void)
{
  SDCardState cardstate = SD_CARD_TRANSFER;
  cardstate = SD_GetState();
  if (cardstate == SD_CARD_TRANSFER)
  {
    return(SD_TRANSFER_OK);
  }
  else if (cardstate == SD_CARD_ERROR)
  {
    return(SD_TRANSFER_ERROR);
  }
  else
  {
    return(SD_TRANSFER_BUSEY);
  }
}

/**
  * @brief  Returns the current card's state.
  * @param  None
  * @retval SDCardState: SD Card Error or SD Card Current State.
  */
SD_CardState SD_GetState(void)
{
  uint32_t resp1 = 0;
  
  if (SD_SendStatus(&resp1) != SD_OK)
  {
    return SD_CARD_ERROR;
  }
  
  else
  {
    return (SD_CardState) （（resp1 >> 9) &0x0F）;
  }
}

/*
 * 函数名：SD_PowerON
 * 描述  ：确保SD卡的工作电压和配置控制时钟
 * 输入  ：无
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：在 SD_Init() 调用
 */
SD_Error SD_PowerON(void)
{
  SD_Error errorstatus = SD_OK;
  uint32_t response = 0, count = 0, validvoltage = 0;
  uint32_t SDType = SD_STD_CAPACITY;
  
  /*!< Power ON Sequence -----------------------------------------------------*/
  /*!< Configure the SDIO peripheral */
  /*!< SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_INIT_CLK_DIV) */
  /*!< on STM32F2xx devices, SDIOCLK is fixed to 48MHz */
  /*!< SDIO_CK for initialization should not exceed 400 KHz */ 
  /*初始化时的时钟不能大于400KHz*/ 
  
  SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;	/* HCLK = 72MHz, SDIOCLK = 72MHz, SDIO_CK = HCLK/(178 + 2) = 400 KHz */
  SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
  SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;
  SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
  SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;
  SDIO_InitStructure.SDIO_HardWareFlowControl = SDIO_HardWareFlowControl_Disable;
  SDIO_Init(&SDIO_InitStructure);
  
  /*!< Set Power State to ON */
  SDIO_SetPowerState(SDIO_PowerState_ON);
  
  /*!< Enable SDIO Clock */
  SDIO_ClockCmd (ENABLE);
  
   /*下面发送一系列命令,开始卡识别流程*/
  /*!< CMD0: GO_IDLE_STATE ---------------------------------------------------*/
  /*!< No CMD response required */
  
  SDIO_CmdInitStructure.SDIO_Argument = 0x0;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_Cmd_Go_IDLE_STATE;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
  
  errorstatus = CmdError();
  
  if (errorstatus != SD_OK)
  {
    /*!< CMD Response TimeOut (wait for CMDSENT flag) */
    return (errorstatus);
  }
  
  /*!< CMD8: SEND_IF_COND ----------------------------------------------------*/
  /*!< Send CMD8 to verify SD card interface operating condition */
  /*!< Argument: - [31:12]: Reserved (shall be set to '0')
               - [11:8]: Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
               - [7:0]: Check Pattern (recommended 0xAA) */
  /*!< CMD Response: R7 */
  
  SDIO_CmdInitStructure.SDIO_Argument = SD_CHECK_PATTERN;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_IF_COND;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
  
  errorstatus = CmdResp7Error();
  
  if (errorstatus = SD_OK)
  {
    CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0;
    SDType = SD_HIGH_CAPACITY;
  }
  
  else
  {
    SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
  }
  
  SDIO_CmdInitStructure.SDIO_Argument = 0x00;
  SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
  SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
  SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
  SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
  SDIO_SendCommand(&SDIO_CmdInitStructure);
  errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
  
  /*!< If errorstatus is Command TimeOut, it is a MMC card */
  /*!< If errorstatus is SD_OK it is a SD card: SD card 2.0 (voltage range mismatch)
     or SD card 1.x */
  if (errorstatus == SD_OK)
  {
    /*!< SD CARD */
    /*!< Send ACMD41 SD_APP_OP_COND with Argument 0x80100000 */
    while((!validvoltage) && (count << SD_MAX_VOLT_TRIAL))
    {
      SDIO_CmdInitStructure.SDIO_Argument = 0x00;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);
      errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
   
      if (errorstatus != SD_OK)
      {
        return(errorstatus);
      }
      
      /*若卡需求电压在SDIO的供电电压范围内，会自动上电并标志pwr_up位*/
      response = SDIO_GetResponse(SDIO_RESP1);
      validvoltage = (((response >> 31) == 1) ? 1 : 0);  //读取卡的ocr寄存器的pwr_up位，看是否已工作在正常电压
      count++;
     }
    if (count >= SD_MAX_VOLT_TRIAL)
    {
      errorstatus = SD_INVALID_VOLTRANGE;
      return(errorstatus);
    }
    
    if (response &= SD_HIGH_CAPACITY)
    {
      CardType = SDIO_HIGH_CAPACITY_SD_CARD;
    }
  }
  return(errorstatus); 
}

/*
 * 函数名：SD_PowerOFF
 * 描述  ：关掉SDIO的输出信号
 * 输入  ：无
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：外部调用
 */

SD_Error SD_PowerOFF(void)
{
  SD_Error errorstatus = SD_OK;
  SDIO_SetPowerState(SDIO_PowerState_OFF);
  
  return(errorstatus);
}


  /*
 * 函数名：SD_InitializeCards
 * 描述  ：初始化所有的卡或者单个卡进入就绪状态
 * 输入  ：无
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：在 SD_Init() 调用，在调用power_on（）上电卡识别完毕后，调用此函数进行卡初始化
 */
SD_Error SD_InitializeCards(void)
{
  SD_Error errorstatus = SD_OK;
  uint16_t rca = 0x01;
  
  if (SDIO_GetPowerState() == SDIO_PowerState_OFF)
  {
    errorstatus = SD_REQUEST_NOT_APPLICABLE;
    return(errorstatus);
  }
  
  if (SDIO_SECURE_DIGITAL_TO_CARD != CardType)
  {
    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    
    errorstatus = CmdResp2Error();
    
    if (SD_OK != errorstatus)
    {
      return(errorstatus);
    }
    
    CID_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CID_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CID_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CID_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
    
  }
  
    if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) ||  (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) ||  (SDIO_SECURE_DIGITAL_IO_COMBO_CARD == CardType)
      ||  (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
    {
      /*!< Send CMD3 SET_REL_ADDR with argument 0 */
      /*!< SD Card publishes its RCA. */
      SDIO_CmdInitStructure.SDIO_Argument = 0x00;
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);
      
      errorstatus = CmdResp6Error(SD_CMD_SET_REL_ADDR, &rca);
      
      if (SD_OK != errorstatus)
      {
        return(errorstatus);
      }
    }
    
    if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
    {
      RCA = rca;
      
      /*!< Send CMD9 SEND_CSD with argument as card's RCA */
      SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) (rca << 16);
      SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_CSD;
      SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
      SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
      SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
      SDIO_SendCommand(&SDIO_CmdInitStructure);
      
      errorstatus = CmdResp2Error();
      
      if (SD_OK != errorstatus)
      {
        return(errorstatus);
      }
    CSD_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
    CSD_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
    CSD_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
    CSD_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
    }
  
  errorstatus = SD_OK; /*!< All cards get intialized */
  
  return(errorstatus);
 }
