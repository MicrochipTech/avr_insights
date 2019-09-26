/*-----------------------------------------------------------------------
/  MMC/SDC Raw Data Interface include file
/  Modified from (C)ChaN, 2014
/-----------------------------------------------------------------------*/

#ifndef _SDCARD_RAW_DEFINED
#define _SDCARD_RAW_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <util/delay.h>

/* Status of Disk Functions */
typedef uint8_t	DSTATUS;


/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Function succeeded */
	RES_ERROR,		/* 1: Disk error */
	RES_NOTRDY,		/* 2: Not ready */
	RES_PARERR		/* 3: Invalid parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (void);
DRESULT disk_read_start (uint32_t sector);
DRESULT disk_read_continue (uint8_t *buff, uint16_t count);
DRESULT disk_read_stop (void);
DRESULT disk_write_start (uint32_t sector);
DRESULT disk_write_continue (uint8_t *buff, uint16_t count);
DRESULT disk_write_stop (void);

#define STA_NOINIT			0x01	/* Drive not initialized */
#define STA_NODISK			0x02	/* No medium in the drive */

#define STD_DATA_TOKEN		0xFE	/* Data token for CMD17/18/24 */
#define WRITE_DATA_TOKEN	0xFC	/* Data token for CMD25 */
#define STOP_TRAN_TOKEN		0xFD	/* Stop transmission token for CMD25 */

#define DATA_RESP_MASK		0x1F
#define DATA_ACCEPTED		0x05
#define DATA_CRC_ERROR		0x0B
#define DATA_WRITE_ERROR	0x0C

#ifdef __cplusplus
}
#endif

#endif	/* _SDCARD_RAW_DEFINED */
