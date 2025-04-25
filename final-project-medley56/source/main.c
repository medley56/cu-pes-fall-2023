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
#include "constants.h"
#include "scmd.h"
#include "encoder.h"
#include "pidctl.h"
#include "pt.h"
#include "tpm.h"

#define UART_BAUD_RATE (115200U)  // baud rate for configuring the UART module


int main(void) {
	/* Initialize board clock and necessary peripherals */
	sysclock_init();  // Initializes clock
	systick_init();  // Initializes systick timer
	led_init();  // Initializes the GPIOs for LED control
	UART0_init(UART_BAUD_RATE);  // Initializes UART0 for serial communication
	printf("UART enabled\r\n");
    encoder_interrupt_init();
    printf("Encoders enabled\r\n");
    scmd_init();
    printf("SCMD initialized\r\n");
    scmd_disable();
    printf("SCMD disabled\r\n");
    pt_init();
    printf("PT detector initialized\r\n");
    scmd_enable();  // Sets enable register in SCMD
    printf("SCMD enabled\r\n");
    tpm1_init();
    printf("Event timer initialized\r\n");
    tpm1_start();
    printf("Event timer running\r\n");

	printf("\n\n\rWelcome to the SCILS command prompt!\r\n");
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
