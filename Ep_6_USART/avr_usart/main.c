/* 
This example project demonstrates the AVR USART.

This example was built for the ATmega324PB Xplained Pro

To test the example open a terminal e.g. PuTTY on the correct COM port at 9600 baud 8/1/0 
*/

#include <avr/io.h>
#include "edbg_serial.h"
#include "main.h"
#include <util/delay.h>


#define BAUDRATE 9600

int main(void)
{

	edbg_serial_init(BAUDRATE);
	edbg_serial_putstring("Hello World!\r\n");
		
	/* LED PC7 active low (output, LED off) */
	PORTC |= (1 << 7);
	DDRC |= (1 << 7);
	
	/* Check button PC6 */
    while (1) 
    {
		if(!(PINC & (1 << 6))){
			PORTC &= ~(1 << 7);
			_delay_ms(100);
			edbg_serial_putstring("Button Pressed\r\n");
			while(!(PINC & (1 << 6)));
			PORTC |= (1 << 7);
			_delay_ms(500);
		}
    }
}

