/*
This example project demonstrates the usage of the AVR ADC noise reduction.

The project is based on the ATMega324PB Xplained Pro.
*/
#include <asf.h>
#include <avr/sleep.h>

#define NOISE_REDUCTION 1

volatile uint8_t adc_buffer_count;
volatile uint16_t adc_buffer[100];

ISR(ADC_vect) {
	adc_buffer[adc_buffer_count++] = ADC;
#if !NOISE_REDUCTION
	ADCSRA |= 1 << ADSC;
#endif
}

static void adc_init(void)
{
	/* Internal 1v1 as ref */
	ADMUX = 0 << REFS1 | 1 << REFS0 | 1 << MUX1 | 1 << MUX2 | 1 << MUX3 | 1 << MUX4; 
	/* Enable ADC and interrupt. Set prescaler to 64 */
	ADCSRA = 1 << ADEN | 1 << ADIE | 1 << ADPS2 | 1 << ADPS1 | 0 << ADPS0;
	/* Disable the digital input on the pin */
	DIDR0 = 1 << ADC1D;
}

int main (void)
{
	board_init();
	
	set_sleep_mode(SLEEP_MODE_ADC);
	
	adc_init();
	sleep_enable();
	sei();
	
#if !NOISE_REDUCTION
	ADCSRA |= 1 << ADSC;
#endif

	while(1) {
		
#if NOISE_REDUCTION
		sleep_cpu();
#endif

		if (adc_buffer_count >= sizeof(adc_buffer)/2) {
			adc_buffer_count = 0;
		}
	}
}
