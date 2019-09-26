#include <stdint.h>

#ifndef TWI_MASTER_H_
#define TWI_MASTER_H_

void twi_master_init(void);
enum twi_status_t twi_master_write(uint8_t slave_address, uint8_t *data, uint16_t data_length);
enum twi_status_t twi_master_read(uint8_t slave_address, uint8_t *data, uint16_t data_length);

enum twi_status_t {
	STATUS_OK,
	STATUS_NACK,
	STATUS_ARB_LOST,
	STATUS_TIMEOUT,
	STATUS_ERROR,
};

enum master_status {
	MASTER_START_SENT                   = 0x08,
	MASTER_REPEATED_START_SENT          = 0x10,
	MASTER_ARB_LOST                     = 0x38,
	MASTER_RECEIVE_SLAVE_ACK_RECEIVED   = 0x40,
	MASTER_RECIEVE_SLAVE_NACK_RECEIVED  = 0x48,
	MASTER_RECEIVE_DATA_ACK_SENT        = 0x50,
	MASTER_RECEIVE_DATA_NACK_SENT       = 0x58,
	MASTER_TRANSMIT_SLAVE_ACK_RECEIVED  = 0x18,
	MASTER_TRANSMIT_SLAVE_NACK_RECEIVED = 0x20,
	MASTER_TRANSMIT_DATA_ACK_RECEIVED   = 0x28,
	MASTER_TRANSMIT_DATA_NACK_RECEIVED  = 0x30,
	
};

#endif /* TWI_MASTER_H_ */