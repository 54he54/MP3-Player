#ifndef __SDIO_SDCARD_H
#define __SDIO_SDCARD_H

#include "stm32f10x.h"

typedef enum
{
  /********SDIO Specific Error Define********/
  SD_CMD_CRC_FAIL           =(1),
  SD_DATA_CRC_FAIL          =(2),
  SD_CMD_RSP_TIMEOUT        =(3),
  SD_DATA_TIMEOUT           =(4),
  SD_TX_UNDERRUN            =(5),
  SD_RX_OVERRUN             =(6),
  SD_START_BIT_ERR          =(7),
  SD_CMD_OUT_OF_RANGE       =(8),
  SD_ADDR_MISALIGENED       =(9),
  SD_BLOCK_LEN_ERR          =(10),
  SD_ERASE_SEQ_ERR          =(11),
  SD_BAD_ERASE_PARAM        =(12),
  SD_WRITE_PORT_VIOLATION   =(13),
  SD_LOCK_UNLOCK_FAILED     =(14),
  SD_COM_CRC_FAILED         =(15),
  SD_CARD_ECC_FAILED        =(17),
  SD_CC_ERROR               =(18),
  SD_GENERAL_UNKONW_ERROR   =(19),
  SD_STREAM_READ_UNDERRUN   =(20),
  SD_STREAM_WRITE_OVERRUN   =(21),
  SD_CID_CSD_OVERWRITE      =(22),
  SD_WP_ERASE_SKIP          =(23),
  SD_CARD_ECC_DISABLED      =(24),
  SD_ERASE_RESET            =(25),
  SD_AKE_SEQ_ERROR          =(26),
  SD_INVALID_VOLTRANGE      =(27),
  SD_ADDR_OUT_OF_RANGE      =(28),
  SD_SWITCH_ERROR           =(29),
  SD_SDIO_DISABLED          =(30),
  SD_SDIO_FUNCTION_BUSY     =(31),
  SD_SDIO_FUNCTION_FAILED   =(32),
  SD_SDIO_UNKNOWN_FUNCTION  =(33),
  
  
  /********Standard error defines********/
  SD_INTERNAL_ERROR,
  SD_NOT_CONFIGURED,
  SD_REQUEST_PENDING,
  SD_REQUEST_NOT_APPLICABLE,
  SD_INVALID_PARAMETER,
  SD_UNSUPPORTED_FEATURE,
  SD_UNSUPPORTED_HW,
  SD_ERROR,
  SD_OK = 0
}SD_ERROR;

  
  /********SDIO Transfer State********/
typedef enum
{
  SD_TRANSFER_OK      = 0,
  SD_TRANSFER_Busy    =1,
  SD_TRANSFER_Error
}SDTransferState;

  
  /********SD Card States********/
typedef enum
{
  SD_CARD_READY               =((uint32_t)0x00000001),
  SD_CARD_IDENTIFICATION      =((uint32_t)0x00000002),
  SD_CARD_STANDBY             =((uint32_t)0x00000003),
  SD_CARD_TRANSFER            =((uint32_t)0x00000004),
  SD_CARD_SENDING             =((uint32_t)0x00000005),
  SD_CARD_RECEIVING           =((uint32_t)0x00000006),
  SD_CARD_PROGRAMMING         =((uint32_t)0x00000007),
  SD_CARD_DISCONNECTED        =((uint32_t)0x00000008),
  SD_CARD_ERROR               =((uint32_t)0x000000FF),
}SDCardState;


  /********SD Card Specific Data: CSD Register********/
typedef struct
{
  __IO uint8_t  CSDStruct;
  __IO uint8_t  SysSpecVersion;
  __IO uint8_t  Reserved1;
  __IO uint8_t  TAAC;
  __IO uint8_t  NSAC;
  __IO uint8_t  MaxBusClkFrec;
  __IO uint16_t  CardComdClasses;
  __IO uint8_t  RDBlockLen;
  __IO uint8_t  PartBlockRead;
  __IO uint8_t  WrBlockMisalign;
  __IO uint8_t  RdBlockMisalign;
  __IO uint8_t  DSRImpl;
  __IO uint8_t  Reserved2;
  __IO uint32_t DeviceSize;
  __IO uint8_t  MaxRdCurrentVDMMin;
  __IO uint8_t  MaxRdCurrentVDMMax;
  __IO uint8_t  MaxWrCurrentVDMMin;
  __IO uint8_t  MaxWrCurrentVDMMax;
  __IO uint8_t  DeviceSizeMul;
  __IO uint8_t  EraseGrSize;
  __IO uint8_t  EraseGrMul;
  __IO uint8_t  WrProtectGrSize;
  __IO uint8_t  WrProtectGrEnable;
  __IO uint8_t  ManDeflECC;
  __IO uint8_t  WrSpeedFact;
  __IO uint8_t  MaxWrBlockLen;
  __IO uint8_t  Reserved3;
  __IO uint8_t  ContentProtectAppli;
  __IO uint8_t  FileFormatGrouop;
  __IO uint8_t  CopyFlag;
  __IO uint8_t  PermWrProtect;
  __IO uint8_t  TempWrProtect;
  __IO uint8_t  FileFormat;
  __IO uint8_t  ECC;
  __IO uint8_t  CSD_CRC;
  __IO uint8_t  Reserved4;
}SD_CSD;


/********Card Identification Data: CID Register********/
typedef struct
{
  __IO uint8_t  ManufacturerID;
  __IO uint16_t  OEMAppliID;
  __IO uint32_t  ProdName1;
  __IO uint8_t  ProdName2;
  __IO uint8_t  ProdRev;
  __IO uint8_t  ProdSN;
  __IO uint8_t  Reserved1;
  __IO uint16_t ManufactDate;
  __IO uint8_t  CID_CRC;
  __IO uint8_t  Reserved2;
}SD_CID;


/********SD Card Status********/
typedef struct
{
  __IO uint8_t    DAT_BUS_WIDTH;
  __IO uint8_t    SECURED_MODE;
  __IO uint16_t   SD_CARD_TYPE;
  __IO uint32_t   SIZE_OF_PROTECTED_AREA;
  __IO uint8_t    SPEED_CLASS;
  __IO uint8_t    PERFORMANCE_MOVE;
  __IO uint8_t    AU_SIZE;
  __IO uint16_t   ERASE_SIZE;
  __IO uint8_t    ERASE_TIMEOUT;
  __IO uint8_t    ERASE_OFFSET;
}SD_CardStatus;


/********SD Card Information********/
typedef struct
{
  SD_CSD    SD_csd;
  SD_CID    SD_cid;
  uint32_t  CardCapacity;
  uint32_t  CardBlockSize;
  uint16_t  RCA;
  uint8_t   CardType;
}SD_CardInfo;




#define     SDIO_FIOF_ADDRESS       ((uint32_t)0x40018080)

/********SDIO Intialization Frequency (400KHZ max)********/
#define SDIO_INIT_CLK_DIV           ((uint8_t)0xB2)

/********SDIO Data Transfer Frequency (25MHz max)********/
/********SDIOCLK = HCLK,SDIO_CK = HCLK/(2 + SDIO_TRANSFER_CLK_DIV)******/
#define SDIO_TRANSFER_CLK_DIV       ((uint8_t0x01))

/******SDIO Commands Index******/
#define SD_CMD_GO_IDLE_STATE                ((uint8_t)0)
#define SD_CMD_SEND_OP_COND                 ((uint8_t)1)
#define SD_CMD_ALL_SEND_CID                 ((uint8_t)2)
#define SD_CMD_SET_REL_ADDR                 ((uint8_t)3)
#define SD_CMD_SET_DSR                      ((uint8_t)4)
#define SD_CMD_SDIO_SEN_OP_COND             ((uint8_t)5)
#define SD_CMD_HS_SWITCH                    ((uint8_t)6)
#define SD_CMD_SEL_DESEL_CARD               ((uint8_t)7)
#define SD_CMD_HS_SEND_EXT_CSD              ((uint8_t)8)
#define SD_CMD_SEND_CSD                     ((uint8_t)9)
#define SD_CMD_SEND_CID                     ((uint8_t)10)
#define SD_CMD_READ_DATA_UNTIL_STOP         ((uint8_t)11)/********SD Card Does't Support It********/
#define SD_CMD_STOP_TRANSMISSION            ((uint8_t)12)
#define SD_CMD_SEND_STATUS                  ((uint8_t)13)
#define SD_CMD_HS_BUSTEST_READ              ((uint8_t)14)
#define SD_CMD_GO_INACTIVE_STATE            ((uint8_t)15)
#define SD_CMD_SET_BLOCKLEN                 ((uint8_t)16)
#define SD_CMD_READ_SINGLE_BLOCK            ((uint8_t)17)
#define SD_CMD_READ_MUTI_BLOCK              ((uint8_t)18)
#define SD_CMD_HS_BUSTEST_WRITE             ((uint8_t)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP         ((uint8_t)20)/********SD Card Does't Support It********/
#define SD_CMD_SET_BLOCK_COUNT              ((uint8_t)23)
#define SD_CMD_WRITE_SINGLE_BLOCK ((uint8_t)24)
#define SD_CMD_WRITE_MULT_BLOCK ((uint8_t)25)
#define SD_CMD_PROG_CID ((uint8_t)26)
#define SD_CMD_PROG_CSD ((uint8_t)27)
#define SD_CMD_SET_WRITE_PORT ((uint8_t)28)
#define SD_CMD_CLR_WRITE_PORT ((uint8_t)29)
#define SD_CMD_SEND_WRITE_PORT ((uint8_t)30)
#define SD_CMD_SD_ERASE_GRP_START ((uint8_t)32)
#define SD_CMD_SD_ERASE_GRP_END ((uint8_t)33)
#define SD_CMD_ERASE_GRP_START ((uint8_t)35)
#define SD_CMD_ERASE_GRP_END ((uint8_t)36)
#define SD_CMD_ERASE ((uint8_t)38)
#define SD_CMD_FAST_IO ((uint8_t)39)/********SD Card Does't Support It********/
#define SD_CMD_GO_IRQ_STATE ((uint8_t)40)/********SD Card Does't Support It********/
#define SD_CMD_LOCK_UNLOCK ((uint8_t)42)
#define SD_CMD_APP_CMD ((uint8_t)55)
#define SD_CMD_GEN_CMD ((uint8_t)56)
#define SD_CMD_NO_CMD ((uint8_t)64)


/*Following cmooands are SD Card Specific commands.
 *SDIO_APP_CMD :CMD55 should be sent before sending these commands.
 */
#define SD_CMD_APP_SD_SET_BUSWIDTH  ((uint8_t)6)  /*For SD Card Only*/
#define SD_CMD_SD_APP_STATUS  ((uint8_t)13)/*For SD Card Only*/
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS ((uint8_t)22)/*For SD Card Only*/
#define SD_CMD_SD_APP_OP_COND ((uint8_t)41)/*For SD Card Only*/
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT ((uint8_t)42)/*For SD Card Only*/
#define SD_CMD_SD_APP_SEND_SCR ((uint8_t)51)/*For SD Card Only*/
#define SD_CMD_SDIO_RW_DIRECT ((uint8_t)52)/*For SD Card Only*/
#define SD_CMD_SDIO_RW_EXTENDED ((uint8_t)53)/*For SD Card Only*/


/*Following commands are SD Card Specific Security commands.
 *SDIO_APP_CMD should be sent before sending these commands.
 */
#define SD_CMD_SD_APP_GET_MKB ((uint8_t)43)/*For SD Card Only*/
#define SD_CMD_SD_APP_GET_MID ((uint8_t)44)/*For SD Card Only*/
#define SD_CMD_SD_APP_SET_CER_RN1 (uint8_t)45)/*For SD Card Only*/
#define SD_CMD_SD_APP_SET_CER_RN2 ((uint8_t)46)/*For SD Card Only*/
#define SD_CMD_SD_APP_SET_CER_RES2 ((uint8_t)47)/*For SD Card Only*/
#define SD_CMD_SD_APP_GET_CER_RES1 ((uint8_t)48)/*For SD Card Only*/
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK ((uint8_t)18)/*For SD Card Only*/
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK ((uint8_t)25)/*For SD Card Only*/
#define SD_CMD_SD_APP_SECURE_ERASE ((uint8_t)38)/*For SD Card Only*/
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA ((uint8_t)49)/*For SD Card Only*/
#define SD_CMD_SD_APP_SECURE_WRITE_MKB ((uint8_t)48)/*For SD Card Only*/


/*Uncomment The Following line to select the SDIO Data transfer mode*/
#define SD_DMA_MODE ((uint8_t)0x00000000)
/* #define SD_POLLING_MODE ((uint8_t)0x00000002)*/


/*SD detection on its memory slot*/
#define SD_PRESENT ((uint8_t)0x01)
#define SD_NOT_PRESENT ((uint8_t)0x00)


/*Supported SD Memory Cards*/
#define SDIO_STD_CAPACITY_SD_CARD_V1_1
  
