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
}
  
