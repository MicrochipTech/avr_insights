/*
This example code demonstrates the usage of the sleep modes on the AVR.
The project is based on the ATMega324PB Xplained Pro.
*/
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

volatile uint8_t WDT_flag = 0;

void WDT_interrupt_enable(void)
{
	/* Timed sequence writing WDCE and WDE. See datasheet chp. 13*/
	WDTCSR |= (1<<WDCE)|(1<<WDE);				//Change enabled
	WDTCSR = (1<< WDIE)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0);	//Configure interrupt with 1 sec timeout	
}

void interrupt_wait( void )
{
	WDT_flag = 0;
	while (!WDT_flag);
}

int main(void)
{
	/* Configure WDT interrupt */
	WDT_interrupt_enable();
	
	/* Enable interrupts */
	sei();

    while (1) 
    {
		/* 
		The example code sets the device to three different sleep modes
		First	set to idle mode, and then wait for WDT interrupt (0,5s)
		Second	set to standby mode, and wait for WDT interrupt (0,5s)
		Third	set to power down three times, to differentiate. And wait for interrupt.
		
		Connect a multimeter to the current measurement header on the XPro.
		*/
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode();
		interrupt_wait();
		
		set_sleep_mode(SLEEP_MODE_STANDBY);
		sleep_mode();	
		interrupt_wait();
		
		/* Three times power down to differentiate */
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();
		interrupt_wait();
		
    }
}

ISR(WDT_vect)
{
	WDT_flag = 1;
}

