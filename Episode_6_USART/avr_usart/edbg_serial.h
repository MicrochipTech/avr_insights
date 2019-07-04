#ifndef EDBG_H_
#define EDBG_H_

#include <stdint.h>

void edbg_serial_init(uint32_t baudrate);

void edbg_serial_putchar(char c);

uint8_t edbg_serial_getchar(char *c, uint16_t retry);

void edbg_serial_putstring(char *string);

void edbg_print_uint16_hex(uint16_t val);

#endif /* EDBG_SERIAL_H_ */