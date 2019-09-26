/*
This example project demonstrates the usage of the AVR power reduction registers

The project is based on the ATMega324PB Xplained Pro.
*/
#include <asf.h>
#include <avr/power.h>
#include <string.h>

/* Used to turn on and off the ADC power reduction */
#define USE_ADC_POWER_REDUCTION 1
/* Used to enable all power reduction bits at startup */
#define USE_ALL_POWER_REDUCTION 1

#define MEAN_BUFFER_SIZE 5

uint16_t mean_buffer[MEAN_BUFFER_SIZE];
uint16_t mean_sort_buf[MEAN_BUFFER_SIZE];
volatile uint16_t mean_value = 0;
uint8_t mean_buffer_count = 0;


static void adc_init(void)
{
	power_adc_enable();
	/* Set ref to AVCC and input to floatin pin */
	ADMUX = 1 << REFS0;
	/* Enable ADC and set prescaler to 32 */
	ADCSRA = 1 << ADEN | 1 << ADPS2 | 1 << ADPS0;
#if USE_ADC_POWER_REDUCTION
	power_adc_disable();
#endif
}

static void adc_trigger(void)
{
	ADCSRA = 1 << ADEN | 1 << ADPS2 | 1 << ADPS0 | 1 << ADSC;
}

static bool adc_get_flag(void)
{
	return !!(ADCSRA & (1 << ADIF));
}

static void adc_clear_flag(void)
{
	ADCSRA = 1 << ADEN | 1 << ADPS2 | 1 << ADPS0 | 1 << ADIF;
}

int main (void)
{
	uint16_t tmp;
	uint16_t min_value;
	uint8_t  min_position = 0;
	/* Disable all clocks */
#if USE_ALL_POWER_REDUCTION
	power_all_disable();
#endif

	/* Set up and enable ADC */
	adc_init();
	
	/* Set up board */
	board_init();
	
	while(1) {
#if USE_ADC_POWER_REDUCTION
		/* Enable the ADC clock */
		power_adc_enable();
#endif
		/* Trigger the ADC conversion */
		adc_trigger();
		/* Wait for conversion to be done */
		while(!adc_get_flag());
		/* Clear flag */
		adc_clear_flag();
		/* Store the ADC conversion */
		tmp = ADC;
#if USE_ADC_POWER_REDUCTION
		/* Disable ADC clock */
		power_adc_disable();
#endif
		
		/* Calculate mean value */
		mean_buffer[mean_buffer_count++] = tmp;
		mean_buffer_count %= MEAN_BUFFER_SIZE;
		
		memcpy(mean_sort_buf, mean_buffer, sizeof(mean_buffer));
		for (uint8_t i = 0; i < MEAN_BUFFER_SIZE; i++) {
			min_value = 0xffff;
			for (uint8_t j = i; j < MEAN_BUFFER_SIZE; j++) {
				if (mean_sort_buf[j] < min_value) {
					min_value = mean_sort_buf[j];
					min_position = j;
				}
			}
			mean_sort_buf[min_position] = mean_sort_buf[i];
			mean_sort_buf[i] = min_value;
		}
		mean_value = mean_sort_buf[MEAN_BUFFER_SIZE/2];
	}
}
