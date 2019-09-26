/* 
This example code demonstrates the usage of the WDT feature on the AVR
The project is based on the ATMega324PB Xplained Pro
*/

#define F_CPU 1000000ul
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#define LED_DDR DDRC
#define LED_PORT PORTC
#define LED_PIN 7 

void LED_init(void)
{
	/* 
	Configure LED0 on the 324PB Xpro.
	Set PIN 7 on PORTC as output and pull pin high.
	*/
	LED_DDR = (1<<LED_PIN);
	LED_PORT |= (1 << LED_PIN);
}

void WDT_sysreset_enable(void)
{
	/* 
	To set WDT in interrupt mode, WDTON fuse must be unprogrammed
	Timed sequence writing WDCE and WDE. See datasheet chp. 13.
	*/
	WDTCSR |= (1<<WDCE) | (1<<WDE);				//Change enabled 
	WDTCSR = (1<<WDE)|(1<<WDP2)|(1<<WDP0);;		//Configure sys. reset with 1 sec timeout
	
}

void WDT_interrupt_enable(void)
{
	/* Timed sequence writing WDCE and WDE. See datasheet chp. 13*/ 
	WDTCSR |= (1<<WDCE)|(1<<WDE);				//Change enabled
	WDTCSR = (1<< WDIE)|(1<<WDP2)|(1<<WDP0);	//Configure interrupt with 1 sec timeout
	
}

int main(void)
{
	/* Initialize LED */
	LED_init();
	
	/*	
	This demo can be configured to use either interrupt or system reset.
	Comment / Uncomment the following functions to test the two modes.
	*/
	WDT_interrupt_enable();
	//WDT_sysreset_enable();
	
	LED_PORT ^= (1<<LED_PIN);
	
	/* Enable global interrupts */
 	sei();
	
    while (1);
}

ISR(WDT_vect)
{
	WDTCSR |= (1<<WDIF);
	LED_PORT ^= (1<<LED_PIN);
}