/**
 * @file UART.h
 * @brief Header file for UART (Universal Asynchronous Receiver/Transmitter) interface functions.
 *
 * This header file contains the declarations and documentation for functions and types used
 * for interfacing with a UART. It includes function prototypes for UART initialization, data transmission,
 * reception, and related utilities.
 *
 * @date 2023-11-06
 * @author Shannon Strutz (original author)
 * @author Alex Dean (modified)
 * @author Gavin Medley (modified)
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>

/**
 * @brief Override __sys_write in order to redirect output functions to UART transmit queue.
 *
 * @param handle Ignored
 * @param buf Character buffer to write/send.
 * @param size Ignored
 */
int __sys_write(int handle, char *buf, int size);

/**
 * @brief Override __sys_readc in order to fetch input from UART receive queue.
 */
int __sys_readc(void);

/**
 * @brief Initialize UART0
 *
 * @param baud_rate Set the baud rate for UART0
 */
void UART0_init(uint32_t baud_rate);

/**
 * @brief Send a string of characters null-terminated over UART.
 *
 * The string gets enqueued into the transmit queue, which is then transmitted via interrupt.
 *
 * @param str The string to send.
 */
void send_string(char * str);

#endif
// *******************************ARM University Program Copyright © ARM Ltd 2013*************************************
