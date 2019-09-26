/*
 * sdcard_raw.c
 * 
 * Created: 24-Nov-16 13:58:28
 *  Author: M43978
 *  Adapted from code by (C)ChaN, 2014
 */ 

#include "sdcard_raw.h"
#include "spi.h"
#include <avr/io.h>

/* Port controls */
#define SELECT()		SPIPORT &= (~SPI_CS)	/* CS = L */
#define	DESELECT()		SPIPORT |= SPI_CS		/* CS = H */
#define SELECTING		!(SPIPORT & SPI_CS)

/* Definitions for SDC commands */
#define GO_IDLE				(0x40+0)	
#define INIT				(0x40+1)
#define	APP_INIT			(0xC0+41)	
#define CHECK_V				(0x40+8)
#define STOP_READ			(0x40+12)
#define SET_BLOCKLEN		(0x40+16)	
#define READ_SINGLE_BLOCK	(0x40+17)	
#define READ_MULTI_BLOCK	(0x40+18)	
#define WRITE_SINGLE_BLOCK	(0x40+24)	
#define WRITE_MULTI_BLOCK	(0x40+25)
#define ACMD_LEADING		(0x40+55)	
#define READ_OCR			(0x40+58)	/* Operation Conditions Register read */


/* Card type flags (CardType) */
#define CT_MMC				0x01	/* MMC ver 3 */
#define CT_SD1				0x02	/* SD ver 1 */
#define CT_SD2				0x04	/* SD ver 2 */
#define CT_BLOCK			0x08	/* Block addressing */

static uint8_t card_type;
static uint16_t read_offset, write_offset;

/******************************************************************************
 * Send a command packet
 ******************************************************************************/
static uint8_t send_cmd(uint8_t cmd, uint32_t arg)
{
	uint8_t i, result;
	
	/* Check ACMD -> sequence of CMD55-CMD */
	if(cmd & 0x80) {
		/* Clear first bit */
		cmd &= 0x7F;	
		/* Send CMD55 */
		result = send_cmd(ACMD_LEADING, 0);
		if(result > 1) return result;
	}
	
	/* Select the card */
	DESELECT();
	rx_spi();
	SELECT();
	rx_spi();
	
	/* Send a command packet */
	tx_spi(cmd);					/* Start + Command index */
	tx_spi((uint8_t)(arg >> 24));		/* Argument[31..24] */
	tx_spi((uint8_t)(arg >> 16));		/* Argument[23..16] */
	tx_spi((uint8_t)(arg >> 8));		/* Argument[15..8] */
	tx_spi((uint8_t)arg);				/* Argument[7..0] */	
	i = 0x01;						/* Dummy CRC + Stop */
	if(cmd == GO_IDLE) i = 0x95; 
	if(cmd == CHECK_V)	i = 0x87;
	tx_spi(i);

	if(cmd == STOP_READ){
		/* Receive stuff byte */		
		result = rx_spi();
	}
	/* Receive a command response */
	/* Try 10 times, command response time is max 8 bytes */
	i = 10;			
	do {
		result = rx_spi();
	} while ((result & 0x80) && --i);

	return result;			/* Return with the response value */
		
}

/******************************************************************************
 * Initialize disk drive
 ******************************************************************************/
DSTATUS disk_initialize(void)
{
	uint8_t i, cmd, type, ocr[4];
	uint16_t timer;
	
	/* Initialize port controls and SPI peripheral */
	init_spi();
	/* 80 dummy clock cycles while chip deselected */
	DESELECT();
	for(i = 10; i; i--) rx_spi();
	
	type = 0;
	/* Go into idle state */
	if(send_cmd(GO_IDLE, 0) == 1) { 
		/* Check voltage */
		if(send_cmd(CHECK_V, 0x1AA) == 1) {
			/* SDv2 */
			type = CT_SD2;
			/* Wait until idle state is left */
			for(timer = 10000; 
				timer && send_cmd(APP_INIT, (1UL << 30)); 
				timer--){
				_delay_us(100);
			}
			/* Check CCS bit */
			if((timer & send_cmd(READ_OCR, 0)) == 0){
				/* Receive OCR */
				for(i = 0; i < 4; i++){
					ocr[i] = rx_spi();
				}
				/* Check if high capacity */
				if(ocr[0] & 0x40){
					type |= CT_BLOCK;
				}
			}
		} else {
			/* SDv1 or MMCv3 */
			if(send_cmd(APP_INIT, 0) <= 1){
				/* SDv1 */
				type = CT_SD1;
				cmd = APP_INIT;
			} else {
				/* MMCv3 */
				type = CT_MMC;
				cmd = INIT;
			}
			/* Wait until idle state is left */
			for(timer = 10000; timer && send_cmd(cmd, 0); timer--){
				_delay_us(100);
			}
		}	
		/* Set R/W block length to 512 */
		if(!timer || send_cmd(SET_BLOCKLEN, 512) != 0){
			type = 0;
		}
	}

	card_type = type;
	DESELECT();
	rx_spi();
	
	if(type == 0){
		return STA_NOINIT;
	} else {
		/* Change SPI clock to be faster (8MHz + DIV4 + 2x = 4MHz) */
		SPCR0 &= ~(1 << SPR1);
		SPSR0 &= ~(1 << SPI2X);
		return 0;
	}
}

/******************************************************************************
 * Read multiple blocks
 ******************************************************************************/

DRESULT disk_read_start(uint32_t sector)
{
	
	DRESULT result;
	uint8_t response;
	
	/* Convert to uint8_t address if needed */
	if(!(card_type & CT_BLOCK))	sector *= 512;
	
	result = RES_ERROR;
	read_offset = 0;
	
	/* Wait until card is ready */
	do{
		response = rx_spi();
	} while(response != 0xFF);
	
	/* Start multiple block read */
	if(send_cmd(READ_MULTI_BLOCK, sector) == 0){
		/* Wait for data token */
		do {
			response = rx_spi();
		} while(response == 0xFF);
		if(response == STD_DATA_TOKEN){
			result = RES_OK;
		}
	}
	
	return result;
}

DRESULT disk_read_continue (uint8_t *buff, uint16_t count)
{
	uint8_t response;
	
	do {
		response = rx_spi();
		*buff++ = response;
		read_offset++;
	} while(--count);
	
	/* Check end of data block */
	if(read_offset == 512){
		/* Read until beginning of next data block */
		do{
			response = rx_spi();
		} while(response != STD_DATA_TOKEN);
		/* Reset offset */
		read_offset = 0;
	} 
	
	return RES_OK;
}

DRESULT disk_read_stop(void)
{
	DRESULT result;
	uint8_t response;
	
	result = RES_ERROR;
	
	if(send_cmd(STOP_READ, 0) == 0){
		do{
			response = rx_spi();
		} while(response != 0xFF);
		result = RES_OK;
	}
	
	DESELECT();
	rx_spi();
	
	return result;
}

/******************************************************************************
 * Write multiple blocks
 ******************************************************************************/

DRESULT disk_write_start (uint32_t sector)
{
	DRESULT result = RES_ERROR;
	uint8_t response;
	
	write_offset = 0;
	
	/* Convert to uint8_t address if needed */
	if(!(card_type & CT_BLOCK)) sector *= 512;
	
	/* Wait until card is ready */
	do{
		response = rx_spi();
	} while(response != 0xFF);
	
	/* Start multiple block read */
	if(send_cmd(WRITE_MULTI_BLOCK, sector) == 0){
		/* Data block header */
		tx_spi(0xFF);
		tx_spi(0xFF);
		tx_spi(WRITE_DATA_TOKEN);
		
		result = RES_OK;
	}
	return result;
}

DRESULT disk_write_continue (uint8_t *buff, uint16_t count)
{
	uint8_t response;
	
	do{
		tx_spi(*buff++);
		write_offset++;
	} while(--count);
	
	if(write_offset == 512){
		/* Send dummy CRC uint8_ts */
		tx_spi(0xFF);
		tx_spi(0xFF);
		
		/* Receive data response */
		do{
			response = rx_spi();		
		} while(response == 0xFF);
		if((response & DATA_RESP_MASK) == DATA_ACCEPTED){
			write_offset = 0;
			/* Wait until end of write process */
			do{
				response = rx_spi();
			} while(response != 0xFF);
			/* Send token ready for next write */
			tx_spi(WRITE_DATA_TOKEN);
		} else {
			return RES_ERROR;
		}
	}
	
	return RES_OK;
}

DRESULT disk_write_stop (void)
{
	uint8_t response;
	
	/* Fill remainder of sector with zeros + CRC */
	if(write_offset){
		while(write_offset < 514){
			tx_spi(0);
			write_offset++;
		}
	}
	/* Receive data response */
	do{
		response = rx_spi();
	} while(response == 0xFF);
	if((response & DATA_RESP_MASK) == DATA_ACCEPTED){
		/* Wait until end of write process */
		do{
			response = rx_spi();
		} while(response != 0xFF);
		/* Stop token + stuff uint8_t */
		tx_spi(STOP_TRAN_TOKEN);
		rx_spi();
		/* Wait for end of write process */
		do{
			response = rx_spi();
		} while(response != 0xFF);
			
		DESELECT();
		return RES_OK;
	} else {
		return RES_ERROR;
	}
}