/*
 * spi.h
 *
 * Created: 06-Dec-16 10:45:00
 *  Author: M43978
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

#define SPIPORT			PORTB
#define SPIDDR			DDRB
#define SPI_SCK			(1 << 7) /* PB7 */
#define SPI_MISO		(1 << 6) /* PB6 SD card DO */
#define SPI_MOSI		(1 << 5) /* PB5 SD card DI */
#define SPI_CS			(1 << 4) /* PB4 */

static inline void init_spi(void)
{
	/* SPI pins */
	SPIDDR |= (SPI_MOSI | SPI_CS | SPI_SCK); /* Set outputs */
	SPIPORT |= SPI_MISO; /* Pull up on SPI_MISO (SD card DO) */

	/* Master Mode, 100-400kHz clk for SD card initialization sequence */	
	/* Setup Peripheral Clock Prescaler for required SD card initialization */
	
	/* SPI Master mode */
	SPCR0 |= (1 << MSTR);
	/* Set clock speed (8MHz oscillator / 32 = 250kHz) */
	SPCR0 |= (1 << SPR1);
	SPSR0 |= (1 << SPI2X);
	/* SPI Enable */
	SPCR0 |= (1 << SPE);
}

static inline uint8_t spi(uint8_t d)
{
	SPDR0 = d;
	while(!(SPSR0 & (1 << SPIF)));
	return SPDR0;
}

static inline void tx_spi (uint8_t d)
{
	spi(d);
}

static inline uint8_t rx_spi (void)
{
	return spi(0xFF);
}


#endif /* SPI_H_ */