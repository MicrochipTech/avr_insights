/*
This example project demonstrates the usage of the ADC triggers on the AVR ADC.
The project is based on the ATMega324PB Xplained Pro.
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define ADC_MUX_SETTING 0x0
#define BUFFER_SIZE 128

uint16_t adc_results[BUFFER_SIZE];
volatile uint8_t adc_results_index = 0;
volatile uint8_t adc_conversion_done = 0;

ISR(ADC_vect)
{
	adc_results[adc_results_index++] = ADC;

	if (adc_results_index >= BUFFER_SIZE) {
		adc_results_index = 0;
		adc_conversion_done = 1;
		/* Disable ADC and clear interrupt flag. we are done */
		ADCSRA = 0;
		ADCSRA = (1 << ADIF);
	}
}

ISR(TIMER0_OVF_vect)
{
	/* Dummy interrupt handler to clear the overflow flag */
}


static void setup_adc_manual_trigger(void)
{
	/* AVcc as reference, ADC0 as input */
	ADMUX = (1 << REFS0) | (ADC_MUX_SETTING);
	/* Enable ADC*/
	ADCSRA = (1 << ADEN);
}

static uint16_t adc_do_manual_conversion(void)
{
	uint16_t result;
	/* Trigger conversion */
	ADCSRA |= (1 << ADSC);

	/* Wait for conversion to complete */
	while (!(ADCSRA & (1 << ADIF)));

	/* Read out result */
	result = ADC;

	/* Clear interrupt flag */
	ADCSRA |= (1 << ADIF);

	return result;
}

static void setup_adc_freerunning_mode(void)
{
	/* AVcc as reference, ADC0 as input */
	ADMUX = (1 << REFS0) | (ADC_MUX_SETTING);

	/* Free running trigger source */
	ADCSRB = 0;

	/* Enable ADC and conversion complete interrupt. Trigger first conversion */
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADATE);

	/* Trigger first conversion */
	ADCSRA |= (1 << ADSC);
}


static void setup_timer_trigger_source(void)
{
	/* Enable Timer counter 0 with prescaler of 1024 */
	TCCR0B = (1 << CS00) | (1 << CS02);
	TIMSK0 = (1 << TOIE0);
}


static void setup_adc_autotrigger(void)
{
	/* AVcc as reference, ADC0 as input */
	ADMUX = (1 << REFS0) | (ADC_MUX_SETTING);

	/* Timer counter 0 overflow as trigger source */
	ADCSRB = (1 << ADTS2);

	/* Enable ADC and conversion complete interrupt. Trigger first conversion */
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADATE);

	/* Trigger first conversion */
	ADCSRA |= (1 << ADSC);
}


int main(void)
{
	uint8_t i;

	/*Do BUFFER_SIZE manual conversions */
	setup_adc_manual_trigger();
	for (i = 0; i < BUFFER_SIZE; i++) {
		adc_results[i] = adc_do_manual_conversion();
	}

	/* Enable interrupts */
	sei();

	/* Do BUFFER_SIZE conversions using the freerunning mode */
	setup_adc_freerunning_mode();

	/* Wait for the conversion to complete */
	while (!adc_conversion_done);

	/* Reset conversion done flag */
	adc_conversion_done = 0;

	/* Enable the timer to use as the autotrigger source */
	setup_timer_trigger_source();
	/* Do BUFFER_SIZE conversions using timer 0 overflow as the trigger source */
	setup_adc_autotrigger();

	while (!adc_conversion_done);

	while (1) {
		asm volatile("nop");
	}





}

