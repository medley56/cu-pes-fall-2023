/**
 * @file main.c
 * @brief Application entry point
 * 
 * @author Howdy Pierce, howdy.pierce@colorado.edu
 * @author Gavin Medley (modified)
 */
#include <assert.h>
#include <stdio.h>
#include "MKL25Z4.h"
#include "sysclock.h"
#include "led.h"
#include "timing.h"
#include "UART.h"
#include "command.h"
#include "test_cbfifo.h"
#include "constants.h"

#define UART_BAUD_RATE (38400U)  // baud rate for configuring the UART module


int main(void) {
	/* Initialize board clock and necessary peripherals */
	sysclock_init();  // Initializes clock
	systick_init();  // Initializes systick timer
	led_init();  // Initializes the GPIOs for LED control
	UART0_init(UART_BAUD_RATE);  // Initializes UART0 for serial communication

#ifdef DEBUG
	/* DEBUG mode tests for cbfifo module */
	assert(test_cbfifo() == 0);
#endif

	printf("\n\n\rWelcome to the Simple Command Interface!\r\n");
	char cmd_buf[MAX_CMD_LEN];
	while (1) {
		if (await_command(cmd_buf) == SUCCESS)
		{
			process_command(cmd_buf);
		} else
		{
			printf("Encountered a problem while awaiting a command.\r\n");
		}
	}

	return 0;
}
