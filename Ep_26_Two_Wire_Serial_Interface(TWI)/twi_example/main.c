#include <avr/io.h>
#include "twi_master.h"

/*
This example demonstrates the usage of the ATmega TWI peripheral
to read out the 9-bit temperature from an AT30TSE758 temperature
sensor. It also reads out the temperature low limit register (factory default 0x4b00)

If using the ATmega324PB Xplained Pro you must connect the IO1 Xplained Pro to EXT1

The TWI driver code can be found in twi_master.c/h
*/


/* I2C Address of the AT30TSE758 on the IO1 Xplained wing */
#define TEMP_SENSOR_I2C_ADDR 0x4f

#define TEMP_SENSOR_TEMP_REG_ADDR 0x0
#define TEMP_SENSOR_TLOW_LIMIT_REG_ADDR 0x12

int main(void)
{
	enum twi_status_t status;
	uint8_t rx_buffer[2];
	uint8_t tx_buffer[1];
	volatile uint16_t temperature_sensor_low_limit = 0xFFFF;
	volatile int16_t temperature_fixed;
	volatile uint8_t *tmp = (uint8_t*)&temperature_sensor_low_limit;
	
	/* Initialize TWI pins and clock prescaler */
	twi_master_init();

	while(1) {
		/* Read temperature low limit register */
		tmp = (uint8_t*)&temperature_sensor_low_limit;
		tx_buffer[0] = TEMP_SENSOR_TLOW_LIMIT_REG_ADDR;
		status = twi_master_write(TEMP_SENSOR_I2C_ADDR, tx_buffer, 1);
		if (status == STATUS_OK) {
			status = twi_master_read(TEMP_SENSOR_I2C_ADDR, rx_buffer,2);
			if (status == STATUS_OK) {
				tmp[0] = rx_buffer[1];
				tmp[1] = rx_buffer[0];
			}
		}

		/* Read out temperature register */
		tmp = (uint8_t*)&temperature_fixed;	
		tx_buffer[0] = TEMP_SENSOR_TEMP_REG_ADDR;
		status = twi_master_write(TEMP_SENSOR_I2C_ADDR, tx_buffer, 1);
		if (status == STATUS_OK) {
			status = twi_master_read(TEMP_SENSOR_I2C_ADDR, rx_buffer, 2);
			if (status == STATUS_OK) {
				tmp[0] = rx_buffer[1];
				tmp[1] = rx_buffer[0];
				temperature_fixed /= 256;
			}
		}
	}
	while(1);	

}

