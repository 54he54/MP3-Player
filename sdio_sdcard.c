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
  GPIO_InitTypeDef
