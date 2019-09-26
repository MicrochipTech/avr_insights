#include <avr/io.h>
#include "twi_master.h"

/* Pin and TWI settings for the ATmega324PB Xplained Pro EXT1 connector */
#define TWI_DDR DDRE
#define TWI_SDA_PIN 5
#define TWI_SCL_PIN 6

#define TWI_BR  TWBR1
#define TWI_SR  TWSR1
#define TWI_AR  TWAR1
#define TWI_DR  TWDR1
#define TWI_CR  TWCR1
#define TWI_AMR TWAMR1


#define TWI_SPEED 400000UL
#define BAUDRATE_VALUE (((F_CPU / TWI_SPEED)-16) / 2)

/* Wait for TWI interrupt flag */
static inline uint16_t twi_wait_for_flag(void)
{
	uint16_t timeout = 0xffff;
	
	/* Wait for interrupt flag to be set or timeout */
	while((!(TWI_CR & (1 << TWINT))) && (timeout--));

	return timeout;
}

/* Get status register and mask out prescaler bits */
static inline uint8_t twi_get_status(void)
{
	/* Mask out prescaler bits */
	return TWI_SR & 0xF8;
}

/* Send a START condition and the slave address */
static inline enum twi_status_t twi_send_start_and_address(uint8_t address)
{
	TWI_CR = (1 << TWEN) | (1 << TWSTA);
	TWI_CR |= (1 << TWINT);

	if (!twi_wait_for_flag()) {
		return STATUS_TIMEOUT;
	}

	if (twi_get_status() != MASTER_START_SENT) {
		return STATUS_ERROR;
	}
	
	/* Clear start bit */
	TWI_CR = (1 << TWEN);

	/* Write slave address */
	TWI_DR = address;
	
	/* Clear interrupt flag to start transmit of slave address */
	TWI_CR |= (1 << TWINT);
	
	if (!twi_wait_for_flag()) {
		return STATUS_TIMEOUT;
	}
	
	return STATUS_OK;	
}

/* Send a STOP condition on the bus */
static inline void twi_send_stop(void)
{
	TWI_CR |= (1 << TWSTO) | (1 << TWINT);
	
	/* Wait until stop has been sent */
	while(TWI_CR & (1 << TWSTO));
}


/* Setup IO pins and TWI prescaler */
void twi_master_init(void)
{
	TWI_DDR = (1 << TWI_SCL_PIN) | (1 << TWI_SDA_PIN);
	TWI_SR = 0;
	TWI_BR = BAUDRATE_VALUE;
}

/* Read data_length bytes from slave to data buffer
  Slave address is the 7-bit address in bit 0-6, no RW bit included.
  
  STATUS_OK - transfer was successful
  STATUS_NACK - slave did not ACK address
  STATUS_TIMEOUT - interrupt flag was not set in time
  STATUS_ARB_LOST - arbitration lost during slave addressing
  STATUS_ERROR    - unhandled error
*/
enum twi_status_t twi_master_read(uint8_t slave_address, uint8_t *data, uint16_t data_length)
{
	enum twi_status_t ret = STATUS_ERROR;
	
	/* Send start condition and slave address */
	ret = twi_send_start_and_address(slave_address << 1 | 0x1);
	
	if (ret != STATUS_OK) {
		return ret;
	}
	
	/* Check for arbitration lost or NACK*/
	switch(twi_get_status()) {
		case MASTER_ARB_LOST:
			return STATUS_ARB_LOST;		
			break;
		case MASTER_RECEIVE_SLAVE_ACK_RECEIVED:
			/* Start reception of first byte */
			if (data_length > 1) {
				TWI_CR |= (1 << TWINT) | (1 << TWEA);
			} else {
				TWI_CR |= (1 << TWINT);
			}
			break;
		case MASTER_RECIEVE_SLAVE_NACK_RECEIVED:
			twi_send_stop();
			return STATUS_NACK;
			break;
		default:
			return STATUS_ERROR;
	}
	
	if (!twi_wait_for_flag()) {
			return STATUS_TIMEOUT;
	}
	
	if (data_length >= 1) {	
		if (twi_get_status() == MASTER_RECEIVE_DATA_ACK_SENT) {
				*(data++) = TWI_DR;
				data_length--;
		}
	
		while(--data_length) {
			
			TWI_CR |= (1 << TWINT) | (1 << TWEA);
		
			if (!twi_wait_for_flag()) {
				return STATUS_TIMEOUT;
			}
			
			if (twi_get_status() == MASTER_RECEIVE_DATA_ACK_SENT) {
				*(data++) = TWI_DR;
			}
		}
		
		/* Clear ACK enable */
		TWI_CR = (1 << TWEN);;
		TWI_CR |= (1 << TWINT);
	
		if (!twi_wait_for_flag()) {
				return STATUS_TIMEOUT;
		}
	}
	
	if (twi_get_status() == MASTER_RECEIVE_DATA_NACK_SENT) {
		*(data++) = TWI_DR;
	}
	
	/* Send stop */
	twi_send_stop();
	
	return STATUS_OK;
}

/* Write data_length bytes from to data buffer to slave
  Slave address is the 7-bit address in bit 0-6, no RW bit included.
  
  STATUS_OK - transfer was successful
  STATUS_NACK - slave did not ACK address
  STATUS_TIMEOUT - interrupt flag was not set in time
  STATUS_ARB_LOST - arbitration lost during slave addressing
  STATUS_ERROR    - unhandled error
*/
enum twi_status_t twi_master_write(uint8_t slave_address, uint8_t *data, uint16_t data_length)
{
	enum twi_status_t ret = STATUS_ERROR;
		
	/* Send start condition and slave address */
	ret = twi_send_start_and_address(slave_address << 1);
		
	if (ret != STATUS_OK) {
		return ret;
	}
		
	/* Check for arbitration lost or NACK*/
	switch(twi_get_status()) {
		case MASTER_ARB_LOST:
			return STATUS_ARB_LOST;
			break;
		case MASTER_TRANSMIT_SLAVE_ACK_RECEIVED:
			break;
		case MASTER_TRANSMIT_SLAVE_NACK_RECEIVED:
			twi_send_stop();
			return STATUS_NACK;
			break;
		default:
			return STATUS_ERROR;
	}
	
	/* Slave ACKED - send data until empty or until NACK is received */	
	while(data_length--) {
		TWI_DR = *(data++);
		TWI_CR |= (1 << TWINT);
		if (!twi_wait_for_flag()) {
			return STATUS_TIMEOUT;
		}
		if (twi_get_status() != MASTER_TRANSMIT_DATA_ACK_RECEIVED) {
			break;
		}	
	}
				
	/* Send stop */
	twi_send_stop();
		
	return STATUS_OK;
}