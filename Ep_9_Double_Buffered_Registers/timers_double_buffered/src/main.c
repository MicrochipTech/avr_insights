/*
This example project demonstrates double buffered registers in the AVR timers.

The project is based on the ATMega324PB Xplained Pro.

For the example, PD5 will toggle on each overflow which is set by the OSC1A compare value.
In both buffered and unbuffered mode, the compare value will be changed at the worst time 
just after it has passed the new value. To visualize the glitch that occurs, a jumper
wire can be put between PD5 and PC7 so that the output is shown on the LED located on
the Xplained pro board. First time button is pressed, you can observe that the LED has
one long period before it blink at the new, faster pace. That's when the glitch occurs.
Press again to go back to the previous speed, at which time there will not be an issue
as the new value is bigger than the last.
*/
#include <asf.h>

#define USE_DOUBLE_BUFFERING 0

#define NORMAL_OVERFLOW_VALUE 7812
#define NORMAL_COMAPRE_VALUE (NORMAL_OVERFLOW_VALUE/2)

#define TRIGGER_WINDOW 100

static void pwm_init(void)
{
	/* Enable output on the PWM pin */
	DDRD = 1 << 5;
	
	/* Set compare values */
	OCR1A = NORMAL_OVERFLOW_VALUE;
	OCR1B = NORMAL_COMAPRE_VALUE;
	
	/* Set wave generation mode based on buffered enabled or not */
#if USE_DOUBLE_BUFFERING
	TCCR1A = 1 << COM1A0 | 1 << WGM00 | 1 << WGM01;
	TCCR1B = 1 << WGM02 | 1 << WGM03 | 1 << CS00 | 1 << CS02;
#else
	TCCR1A = 1 << COM1A0;
	TCCR1B = 1 << WGM02 | 1 << CS00 | 1 << CS02;
#endif
}

int main (void)
{
	uint8_t state = 0;
	pwm_init();
	ioport_configure_pin(GPIO_PUSH_BUTTON_0, IOPORT_DIR_INPUT | IOPORT_PULL_UP);
	
	while (1) {
		/* Wait for pin change */
		if (ioport_get_pin_level(GPIO_PUSH_BUTTON_0) == 0) {
			switch (state) {
				case 0:
					while(!((TCNT1 > OCR1B) && (TCNT1 < (OCR1B+TRIGGER_WINDOW))));
					OCR1A = NORMAL_COMAPRE_VALUE;
					OCR1B = NORMAL_COMAPRE_VALUE / 2;
					break;
				case 1:
					OCR1A = NORMAL_OVERFLOW_VALUE;
					OCR1B = NORMAL_COMAPRE_VALUE;
					break;
				default:
					break;
			}
			
			
			while(ioport_get_pin_level(GPIO_PUSH_BUTTON_0) == 0);
			state++;
			state %= 2;
		}
	}
}
