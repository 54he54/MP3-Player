#ifndef __SDIO_SDCARD_H
#define __SDIO_SDCARD_H

#include "stm32f10x.h"

typedef enum
{
  SD_CMD_CRC_FAIL       =(1),
  SD_DATA_CRC_FAIL      =(2),
  SD_CMD_RSP_TIMEOUT    =(3),
  SD_DATA_TIMEOUT       =(4),
  SD_TX_UNDERRUN        =(5),
  SD_RX_OVERRUN         =(6),
  SD_START_BIT_ERR      =(7),
  SD_CMD_OUT_OF_RANGE   =(8),
  
