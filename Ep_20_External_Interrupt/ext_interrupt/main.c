/*
This example code demonstrates how to configure the sleep mode 
with ext. interrupt as wake-up source, turn on an LED for one second
and then go back to sleep.

The project is based on the ATMega324PB Xplained Pro.

*/

#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED_DDR DDRC
#define LED_PORT PORTC
#define LED_PIN 7

#define SW_DDR DDRC
#define SW_PORT PORTC
#define SW_PIN 6

volatile uint8_t button_flag = 0;

void init_led()
{
	/* Configure LED as output */
	LED_DDR = (1<<LED_PIN);
	LED_PORT = (1<<LED_PIN);
}

void init_button()
{
	/* Configure SW (PC6) as input */
	SW_DDR &= ~(1<<SW_PIN);
	SW_PORT |= (1<<SW_PIN);
}
	
void init_extint()
{
	/* SW connected to PCINT22. Enable pin changer interrupt */
	PCICR = 1<<PCIE2;
	PCMSK2 = 1<<PCINT22;
}

void init_sleep()
{
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

int main(void)
{
	init_led();
	init_button();
	init_extint();
	init_sleep();
	sei();
	
    while (1) 
    {
		/*
		When button interrupt occurs, the button flag is set.
		The LED_PIN is pulled low for one second (LED ON), and then pulled high (LED OFF).
		*/
		if (button_flag == 1)
		{
			LED_PORT &= ~(1<<LED_PIN);
			_delay_ms(1000);		
			LED_PORT |= (1<<LED_PIN);
			button_flag = 0;		
		}
		sleep_mode();
    }
}

ISR(PCINT2_vect)
{
	button_flag = 1;
}
