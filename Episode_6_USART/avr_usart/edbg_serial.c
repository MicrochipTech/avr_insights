#include <avr/io.h>
#include <stdint.h>
#include "main.h"
#include "edbg_serial.h"

/* ATmega324PB Xplained Pro */
#ifdef __AVR_ATmega324PB__
  #define EDBG_UDR UDR1
  #define EDBG_UCSRA UCSR1A
  #define EDBG_UCSRB UCSR1B
  #define EDBG_UCSRC UCSR1C
  #define EDBG_UCSRD UCSR1D
  #define EDBG_UBRR UBRR1
  
  #define EDBG_TX_PORT_DDR DDRD
  #define EDBG_TX_PIN 3
  #define EDBG_RX_PORT_DDR DDRD
  #define EDBG_RX_PIN 2
#endif 
  
void edbg_serial_init(uint32_t baudrate)
{
	uint16_t ubrr = (F_CPU / 8 / baudrate) - 1;
	
	EDBG_UBRR = ubrr;
	
	EDBG_UCSRB = (1 << TXEN) | (1 << RXEN);
	/* 8-bit character */
	EDBG_UCSRC = (3 << 1);
	
	EDBG_UCSRA = (1 << U2X);
	/* Set TX pin as output */
	EDBG_TX_PORT_DDR = (1 << EDBG_TX_PIN);
	
}

void edbg_serial_putchar(char c)
{
	while(!(EDBG_UCSRA & (1 << UDRE)));
	EDBG_UDR = c;
}


void edbg_serial_putstring(char *string)
{
	while(*string) {
		edbg_serial_putchar(*string++);
	}
}

uint8_t edbg_serial_getchar(char *c, uint16_t retry)
{
	if (retry) {
		while(retry-- && !(EDBG_UCSRA & (1 << RXC)));
	} else {
		retry = 0xff;
		while(!(EDBG_UCSRA & (1 << RXC)));
	}
	
	if (retry == 0) {
		return 0;
	} else {
		*c = EDBG_UDR;
		return 1;
	}
}



void edbg_print_uint16_hex(uint16_t val) {
	uint8_t tmp;
	int8_t i;
	for (i = 3; i >= 0; i--) {
		tmp = (val >> (i * 4));
		tmp &= 0xf;
		if (tmp > 9) {
			tmp += 0x61 - 10;
		} else {
			tmp += 0x30;
		}
		edbg_serial_putchar(tmp);
	}
	
}