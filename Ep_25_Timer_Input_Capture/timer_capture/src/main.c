/*
This example project demonstrates the timer capture module on the AVR timers.

The project is based on the ATMega324PB Xplained Pro.

For this example, make sure that the device fuses are set for the system clock
to match the define below. The output PWM and input capture is on the same pin,
so no extra hardware is needed.

Note: To measure duty cycle the trigger edge needs to be toggled after each edge
transition. If the sampled signal is to fast compared to the timer frequency, 
the transition could be missed.

*/

#include <asf.h>

#define OUTPUT_DDR DDRD
#define OUTPUT_PIN 6

#define SYSTEM_SPEED 8000000
#define CAPTURE_TIMER_PRESCALER 1
#define CAPTURE_TIMER_SPEED (SYSTEM_SPEED/CAPTURE_TIMER_PRESCALER)

uint8_t capture_state = 0;
uint32_t current_start_value = 0;
uint16_t last_start_value = 0;
uint16_t last_dc_value = 0;
uint16_t overflow_value = 0;

/* Variable to store measured frequency */
uint32_t freq = 0;
/* Variable to store measured duty cycle */
uint32_t duty = 0;
ISR(TIMER1_CAPT_vect) 
{
	if (capture_state == 0) {
		/* Save capture value. Compensate for overflow */
		current_start_value = ICR1 + overflow_value;
		/* Change polarity of trigger */
		TCCR1B = 1 << CS10 | 0 << ICES1;
		/* Calculate frequency */
		freq = CAPTURE_TIMER_SPEED / (current_start_value - last_start_value);
		duty = ((last_dc_value - last_start_value) * freq * 100) / CAPTURE_TIMER_SPEED;

		/* Save last capture to use as start for next period */
		last_start_value = current_start_value & 0xffff;
		/* Change state for reading other polarity */
		capture_state = 1;
	} else {
		/* Save capture value. Compensate for overflow */
		last_dc_value = ICR1  + overflow_value;
		/* Change polarity of trigger */
		TCCR1B = 1 << CS10 | 1 << ICES1;
		
		capture_state = 0;
	}
	overflow_value = 0;
}

/* Overflow interrupt to save when overflow occurs */
ISR(TIMER1_OVF_vect) 
{
	overflow_value = 0xffff;
}

static void init_output_timer(void)
{
	/* Fast PWM mode */
	TCCR2A = 1 << COM2B1 | 1 << WGM21 | 1 << WGM20;
	/* Fast PWM mode, prescaler div 64 */
	TCCR2B = 1 << WGM22 | 1 << CS22;
	
	/* 1Mhz / 64 / 125 = 1000hz */
	OCR2A = 124;
	/* Some duty cycle value ~24% */
	OCR2B = 30;
	/* Set PWM pin as output */
	OUTPUT_DDR = 1 << OUTPUT_PIN;
}

static void init_capture_timer(void)
{
	/* Prescaler = 8, Start with rising edge on waveform */
	TCCR1B = 1 << CS11 | 1 << ICES1;
	/* Enable interrupt flag */
	TIMSK1 = 1 << ICIE1;
}

int main (void)
{

	init_capture_timer();
	init_output_timer();
	
	sei();

	while (1)
	{
	}
}
