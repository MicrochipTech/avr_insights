/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * For the example to work currently, make sure that the BOOTRST fuse is set in the device. Otherwise the device will
 * start execute application from start of the flash, and not from the boot section where this code is put.
 */
#include <asf.h>
#include <avr/boot.h>

#define DEVICE_PAGE_SIZE 128
#define DEVICE_APPLICATION_SECTION_LENGTH 0x7000

/* Set this to one to simulate not having read while write in the device */
#define USE_BUSY_WAIT_SPM 0

#define BOOT_STATE_ERASE				0
#define BOOT_STATE_FILL_PAGEBUFFER		1
#define BOOT_STATE_STATUS				2
#define BOOT_STATE_WRITE				3
#define BOOT_STATE_FIB					4
#define BOOT_STATE_DONE_AND_ENABLE_RWW	5
#define BOOT_STATE_DONE					6

const uint8_t test_pattern_data[DEVICE_PAGE_SIZE] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f
};

static int fib(int n)
{
	if ( n <= 1 ) return n;
	return fib(n-1) + fib(n-2);
}

int main (void)
{
	/* Variable used to set address to operate on in flash */
	uint16_t flash_address = 0;
	int fibonacci_number;
	/* Variable used to see the difference between having a read while write section and not. */
	/* Having a read while write section allows some kind of operation while waiting */
	/* for the flash to finish erase/write. The more time the device has while waiting for    */
	/* the flash, the higher the number will be */
	volatile uint32_t calculations = 0;
	
	uint8_t state = 0, next_state = 0;
	
	board_init();
	
	while (1) {
		switch(state) {
			case BOOT_STATE_ERASE:
				boot_page_erase(flash_address);

#if USE_BUSY_WAIT_SPM
				boot_spm_busy_wait();
#endif

				next_state = BOOT_STATE_FILL_PAGEBUFFER;
				state = BOOT_STATE_STATUS;
				break;
			case BOOT_STATE_FILL_PAGEBUFFER:
				boot_page_fill(flash_address, test_pattern_data[flash_address%DEVICE_PAGE_SIZE] | test_pattern_data[(flash_address+1) % DEVICE_PAGE_SIZE] << 8);

#if USE_BUSY_WAIT_SPM
				boot_spm_busy_wait();
#endif

				flash_address += 2;
				if (!(flash_address % DEVICE_PAGE_SIZE)) {
					next_state = BOOT_STATE_WRITE;
					state = BOOT_STATE_STATUS;
				}
				break;
			case BOOT_STATE_STATUS:
				if (!boot_spm_busy()) {
					state = next_state;
				} else {
					state = BOOT_STATE_FIB;
				}
				break;
			case BOOT_STATE_WRITE:

				boot_page_write(flash_address-DEVICE_PAGE_SIZE);
#if USE_BUSY_WAIT_SPM
				boot_spm_busy_wait();
#endif

				if (flash_address >= DEVICE_APPLICATION_SECTION_LENGTH) {
					state = BOOT_STATE_STATUS;
					next_state = BOOT_STATE_DONE_AND_ENABLE_RWW;
				} else {
					state = BOOT_STATE_ERASE;
				}
				break;
			case BOOT_STATE_FIB:
				/* Do something useful! In this example, Fibonacci numbers are calculated (Not useful) */
				calculations++;
				fibonacci_number = fib(5);
				state = BOOT_STATE_STATUS;
				break;
			case BOOT_STATE_DONE_AND_ENABLE_RWW:
				boot_rww_enable();
				state = BOOT_STATE_DONE;
				break;
			default:
				break;
		}
	}
}
