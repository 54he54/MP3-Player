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
