/*
This example project demonstrates differential measurement using the AVR ADC.

The project is based on the ATMega324PB Xplained Pro.
*/

#include <asf.h>

static bool adc_data_is_ready(void)
{
	return !!(ADCSRA & (1 << ADIF));
}

static void adc_data_clear_flag(void)
{
	ADCSRA |= 1 << ADIF;
}
static void adc_start_conversion(void)
{
	ADCSRA |= 1 << ADSC;
}

static uint16_t adc_get_data(void)
{
	return ADC;
}

static void adc_init(void)
{
	/* Differential ADC input on pin 0 with 200x gain and reference 2v56*/
	ADMUX = 1 << MUX1 | 1 << MUX3 | 1 << REFS1 | 1 << REFS0;
	/* Enable ADC and set prescaler for an appropriate value */
	ADCSRA = 1 << ADEN | 1 << ADPS2 | 1 << ADPS1;
	
	/* Disregard first sample */
	adc_start_conversion();
	while(!adc_data_is_ready());
	adc_data_clear_flag();
}

volatile uint16_t adc_value;
int main (void)
{
	board_init();

	adc_init();
	
	

	while (1) {
		adc_start_conversion();
		while(!adc_data_is_ready());
		/* ADC value can be calculated by: V = adc_value/1024*1.1/200
		 * Given: reference = 1v1 and gain of 200 */
		adc_value = adc_get_data();
		adc_data_clear_flag();
	}
}
