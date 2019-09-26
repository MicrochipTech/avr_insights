#include <avr/io.h>
#include <stdint.h>
#include "edbg_serial.h"
#include "main.h"
#include <util/delay.h>

/* Flash read/write */
#include <avr/pgmspace.h>
#include <avr/boot.h>

#include <avr/wdt.h>


/* EEPROM */
#include <avr/eeprom.h>

const __flash uint32_t serial_number = 0x12345678;

/* Default bootloader size is 2048 words (4096 bytes) */
#define FLASH_BOOT_SIZE (2048 * 2)
#define FLASH_RWW_START (32768 - 4096)

/* Place counter in flash at the end of the application flash area */
#define FLASH_COUNTER_ADDRESS (0x7000 - 2)

#define EEPROM_COUNTER_ADDRESS (E2START)

/* This function uses the helper functions in pgmspace.h to read data from flash memory */
void read_data_from_flash(uint16_t address, uint8_t *data, uint16_t len)
{
	while(len--) {
		*data++ = pgm_read_byte(address++);
	}
}

/* This functions uses the functions in avr/boot.h to write to the flash memory. The function is placed in the bootloader area of the flash
   to allow writing to the flash.
   The .boot section is setup in Project -> <project name> Properties -> Toolchain -> AVR/GNU Linker - Memory settings
   Notice that the .boot section has a word (16-bit) address, and not byte address when added to the Memory settings */
__attribute__ ((section(".boot"))) void write_data_to_flash(uint16_t address, uint8_t *data, uint16_t len)
{
	uint8_t page_buffer[SPM_PAGESIZE];
	uint8_t page = address / SPM_PAGESIZE;
	uint8_t page_offset = address % SPM_PAGESIZE;
	uint8_t i;
	uint16_t w;
	do {
		/* Read page from flash */
		read_data_from_flash(page * SPM_PAGESIZE, page_buffer, SPM_PAGESIZE);
	
		/* Modify data */
		while((page_offset < SPM_PAGESIZE) && len) {
			page_buffer[page_offset] = *data++;
			len--;
			page_offset++;
		}

		/* Erase page */
		boot_page_erase (page * SPM_PAGESIZE);
		boot_spm_busy_wait ();

		/* Write buffer to hardware page buffer */
		for (i = 0; i < SPM_PAGESIZE; i+=2) {
			w = page_buffer[i] | page_buffer[i+1] << 8;
			boot_page_fill ((page * SPM_PAGESIZE) + i, w);
		}
		/* Write page to flash */
		boot_page_write(page * SPM_PAGESIZE);
		boot_spm_busy_wait();
				
		page++;
	} while(len);

}


void read_data_from_eeprom(uint16_t address, uint8_t *data, uint16_t len)
{
	eeprom_read_block(data, (void*)address, len);
}

void write_data_to_eeprom(uint16_t address, uint8_t *data, uint16_t len)
{
	eeprom_write_block(data, (void*)address, len);
}

int main(void)
{
	uint16_t flash_counter = 0;
	uint16_t eeprom_counter = 0;
	char c;
	volatile uint32_t serial = serial_number;
	
	/* Read counter from flash memory */
	read_data_from_flash(FLASH_COUNTER_ADDRESS, (uint8_t*)&flash_counter, 2);
	
	/* Erased flash memory have all bits set to 1 (0xffff), if this is the case we reset the counter to zero */
    if (flash_counter == 0xffff) {
		flash_counter = 0;
	}
	/* Increment the counter and write it to flash */
	flash_counter++;
	write_data_to_flash(FLASH_COUNTER_ADDRESS, (uint8_t*)&flash_counter, 2);
	

	/* Read counter from EEPROM, increment and write back to EEPROM */
	read_data_from_eeprom(EEPROM_COUNTER_ADDRESS, (uint8_t*)&eeprom_counter, 2);
	eeprom_counter++;
	write_data_to_eeprom(EEPROM_COUNTER_ADDRESS,(uint8_t*)&eeprom_counter, 2);
	


	edbg_serial_init(9600);
	
	edbg_serial_putstring("Flash counter: 0x");
	edbg_print_uint16_hex(flash_counter);
	edbg_serial_putstring("\r\n");
	
	edbg_serial_putstring("EEPROM counter: 0x");
	edbg_print_uint16_hex(eeprom_counter);
	edbg_serial_putstring("\r\n");
	

	/* Setup LED pins (output pin, LED off) */
	PORTC |= (1 << 7);
	DDRC |= (1 << 7);
	
	/* Flash the LED connected to PORTC pin 7 the same number of times as the device has been reset */
	while(flash_counter--) {
		PORTC &= ~(1 << 7);
		_delay_ms(100);
		PORTC |= (1 << 7);
		_delay_ms(100);
	}


	
	
	while(1) {
		if (edbg_serial_getchar(&c,0)) {
			edbg_serial_putchar(c);
		}
	}
}

