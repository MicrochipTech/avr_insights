/*
This example project demonstrates AVR pin configuration.

The project is based on the ATMega324PB Xplained Pro.
*/

#define F_CPU	1000000	

#include <avr/io.h>
#include <util/delay.h>

#define SWITCH_SAFELY	0

#define SWITCH_PIN		6
#define LED_PIN			7

void switch_to_output_high_safe(void);
void switch_to_output_high_unsafe(void);
void reset_to_default_state(void);

int main(void)
{
    /* Default value on startup is input, no pull up enabled.
	 * LED represents an external component with active low enable.
	 * The LED turning on represents an illegal state
	 */
	
    while (1) 
    {
		if(!(PINC & (1 << SWITCH_PIN))){

			if(SWITCH_SAFELY){
				switch_to_output_high_safe();
			} else {
				switch_to_output_high_unsafe();
			}
			
		}
    }
}

void switch_to_output_high_safe(void)
{
	/* First enable pull up on input pin */
	PORTC |= (1 << LED_PIN);
	/* Wait so we can see that no illegal state is entered (LED is off) */
	_delay_ms(500);
	/* Then change direction to output, PORT value will make output high */
	DDRC |= (1 << LED_PIN);
	
	reset_to_default_state();
}

void switch_to_output_high_unsafe(void)
{
	/* Changing direction first will make output low */
	DDRC |= (1 << LED_PIN);
	/* Wait so we can see the illegal state (LED is on) */
	_delay_ms(500);
	/* Change to output high */
	PORTC |= (1 << LED_PIN);	
	
	reset_to_default_state();
}

void reset_to_default_state(void)
{
	DDRC &= ~(1 << LED_PIN);
	PORTC &= ~(1 << LED_PIN);	
}
