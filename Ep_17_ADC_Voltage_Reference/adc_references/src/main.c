/*
This example project demonstrates AVR ADC references.

The project is based on the ATMega324PB Xplained Pro.

Code for using the different ADC references available on the ATMEGA324PB.
In the example, the input is set to the internal 1v1 bandgap, but if some
external signal is applied, this can also be set to be measured instead of
the bandgap. Note that the first sample must be disregarded.
*/
#include <asf.h>

/* Selectable references. AREF needs to be externally connected to be used. */
#define ADC_REFERENCE_AREF 0
#define ADC_REFERENCE_VCC  1
#define ADC_REFERENCE_1v1  2
#define ADC_REFERENCE_2v56 3

#define ADC_REFERENCE ADC_REFERENCE_VCC

static void adc_init(void)
{
	/* Set the input to internal bandgap. Can also be set to an external pin. */
	ADMUX = 0 << MUX0 | 1 << MUX1 | 1 << MUX2 | 1 << MUX3 | 1 << MUX4;
#if ADC_REFERENCE == ADC_REFERENCE_AREF
	ADMUX |= 0 << REFS0 | 0 << REFS1;
#elif ADC_REFERENCE == ADC_REFERENCE_VCC
	ADMUX |= 1 << REFS0 | 0 << REFS1;
#elif ADC_REFERENCE == ADC_REFERENCE_1v1
	ADMUX |= 0 << REFS0 | 1 << REFS1;
#elif ADC_REFERENCE == ADC_REFERENCE_2v56
	ADMUX |= 1 << REFS0 | 1 << REFS1;
#endif
	/* 8e6 MHz / 64 = 125kHz clock for ADC */
	ADCSRA = 1 << ADEN | 1 << ADPS1 | 1 << ADPS2;
}

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

volatile uint16_t adc_value;
int main (void)
{
	board_init();
	
	adc_init();

	
	while (1) {
		adc_start_conversion();
		while(!adc_data_is_ready());
		/* Save last measurement, value can be read in the watch window. Measured voltage can be
		 * calculated as V = adc_value / 1024 * Vref
		 */
		adc_value = adc_get_data();
		adc_data_clear_flag();
	}
}
