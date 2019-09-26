/*
This example project demonstrates the AVR Real Time Clock

The project is base on the ATMega324PB Xplained Pro.

For this example, make sure to set the fuses to use the 128khz internal RC as system clock.
When running from the internal 128KHz, the overflow value of the asynchronous timer should be 102 for one second tick.
If using an external crystal the value should be 31.
*/

#include <asf.h>
#define USE_EXTERNAL_CRYSTAL 1

#define OUTPUT_PIN 0
#define OUTPUT_DDR DDRA
#define OUTPUT_PORT PORTA

ISR (TIMER2_COMPA_vect) {
	/* Toggle output pin */
	OUTPUT_PORT ^= 1 << OUTPUT_PIN;
}

static void init_ac_timer(void)
{
	/* CTC mode for setting top value */
	TCCR2A = 1 << WGM21;
	/* Prescaler = div by 1024 */
	TCCR2B = 1 << CS20 | 1 << CS21 | 1 << CS22;
#if USE_EXTERNAL_CRYSTAL
	/* Set to external clock */
	ASSR = 1 << AS2;
	/* 32768/1024/32 = 1. Gives interrupt every sec */
	OCR2A = 31;
#else
	/* 105500/1024/103 = 1. Gives interrupt every sec */
	/* Using numbers from typical measured frequency of the oscillator ~=105500Hz. */
	OCR2A = 102;
#endif
	/* Enable interrupt for overflow */
	TIMSK2 = 1 << OCIE2A;
}

int main (void)
{
	/* Init timer */
	init_ac_timer();
	/* Enable output on output pin */
	OUTPUT_DDR = 1 << OUTPUT_PIN;
	
	sei();
	while (1) {
		
	}
}
