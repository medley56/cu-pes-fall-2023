/**
 * @file UART.c
 * @brief Implementation file for UART (Universal Asynchronous Receiver/Transmitter) interface functions.
 *
 * Code sourced from "Embedded Systems Fundamentals with Arm Cortex-M based Microcontrollers"
 * by Alexander G. Dean
 *
 * @date 2023-11-06
 * @author Shannon Strutz (original author)
 * @author Alex Dean (modified)
 * @author Gavin Medley (modified)
 * @see UART.h
 */

#include "UART.h"
#include "MKL25Z4.h"
#include <stdio.h>
#include "cbfifo.h"
#include "timing.h"

#define UART_OVERSAMPLE_RATE (16U)
#define USE_TWO_STOP_BITS (1U)  // 0 for one stop bit, 1 for two stop bits
#define USE_PARITY (1U)  // 0 to not use parity (PE register bit)
#define USE_ODD_PARITY (1U)  // 0 to use even parity (PT register bit). Does not apply if USE_PARITY is 0
#define USE_UART_INTERRUPTS (1U) // 0 for polled UART communications, 1 for interrupt-driven
#define ERROR (0xFFFFFFFFU)
#define SUCCESS (0U)

/* DON'T MODIFY THIS #if BLOCK!
 * Due to the semantics of the UART documentation, the parity bit
 * is included in the number of data bits specified by the M register field.
 * If parity is enabled (odd or even), we enable this additional "data bit"
 * so we don't lose the last bit of our actual data.
 * See section 40.2.3 of the KL25Z reference manual. */
#if USE_PARITY == 1U
#define UART_DATA_MODE (1U)  // use 9 bits for "data". Parity bit is included in this length.
#else
#define UART_DATA_MODE (0U)  // use 8 bits for "data".
#endif

/* BEGIN - UART0 Device Driver

 Code created by Shannon Strutz
 Date : 5/7/2014
 Licensed under CC BY-NC-SA 3.0
 http://creativecommons.org/licenses/by-nc-sa/3.0/

 Modified by Alex Dean 9/13/2016
 Modified by Gavin Medley 11/11/2023
 */

int __sys_write(int handle, char *buf, int size)
{
	send_string(buf);
	return SUCCESS;  // 0 for success
}

int __sys_readc(void)
{
	char rx_char;
	if (cbfifo_dequeue(Q_RX, &rx_char, 1))
	{
		return rx_char;
	}
	return ERROR;  // No characters available to read in the queue
}

// Code listing 8.8, p. 231
void UART0_init(uint32_t baud_rate)
{
	uint16_t sbr;

	// Enable clock gating for UART0 and Port A
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

	// Make sure transmitter and receiver are disabled before init
	UART0->C2 &= ~UART0_C2_TE_MASK & ~UART0_C2_RE_MASK;

	// Set UART clock to 24 MHz clock (MCGFLLCLK)
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1); // 1 MCGFLLCLK clock or MCGPLLCLK/2 clock
	SIM->SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;  // 0 MCGFLLCLK clock

	// Set pins to UART0 Rx and Tx
	PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Rx
	PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2); // Tx

	// Set baud rate and oversampling ratio
	sbr = (uint16_t) ((CLK_FREQ) / (baud_rate * UART_OVERSAMPLE_RATE));
	UART0->BDH = 0;  // Reset BDH register values to zero
	UART0->BDH |= UART0_BDH_SBR(sbr >> 8);
	UART0->BDL = UART0_BDL_SBR(sbr);
	UART0->C4 |= UART0_C4_OSR(UART_OVERSAMPLE_RATE-1);

	// Set two stop bits
	UART0->BDH |= UART_BDH_SBNS(USE_TWO_STOP_BITS);  // 1 two stop bits, 0 one stop bit

	// Disable interrupts for RX active edge and LIN break detect, select one stop bit
	UART0->BDH |=
	UART0_BDH_RXEDGIE(0) | UART0_BDH_SBNS(0) | UART0_BDH_LBKDIE(0);

	// Don't enable loopback mode, use 9 data bit mode (to account for the parity bit), enable parity, use odd parity type
	UART0->C1 = UART0_C1_LOOPS(0) | UART0_C1_M(UART_DATA_MODE) | UART0_C1_PE(USE_PARITY) | UART0_C1_PT(USE_ODD_PARITY);
	// Don't invert transmit data, don't enable interrupts for errors
	UART0->C3 = UART0_C3_TXINV(0) | UART0_C3_ORIE(0)| UART0_C3_NEIE(0)
	| UART0_C3_FEIE(0) | UART0_C3_PEIE(0);

	// Clear error flags
	UART0->S1 = UART0_S1_OR(
			1) | UART0_S1_NF(1) | UART0_S1_FE(1) | UART0_S1_PF(1);

	// Enable interrupts. Listing 8.11 on p. 234
	/* Ensure the circular buffer fifo queues are initialized */
	cbfifo_init();
	NVIC_SetPriority(UART0_IRQn, 3); // 0, 1, 2, or 3
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
	/* Enable receive interrupts but not transmit interrupts yet.
	 * Transmit interrupts are enabled immediately before transmitting
	 * data since TIE is disabled as soon as data transmission completes. */
	UART0->C2 |= UART_C2_RIE(1);

	// Enable UART receiver and transmitter
	UART0->C2 |= UART0_C2_RE(1) | UART0_C2_TE(1);

	// Clear the UART RDRF flag
	UART0->S1 &= ~UART0_S1_RDRF_MASK;

}

// UART0 IRQ Handler. Listing 8.12 on p. 235
void UART0_IRQHandler(void)
{
    uint32_t pm = __get_PRIMASK();
    __disable_irq();

	char ch;

	/* Check for any error flags present
	 * OR = Receiver Overrun. New data is received but previous received byte
	 *      is still in the data register. New data is lost.
	 * NF = Noise Flag. Noise detected while sampling at least one of the bits in
	 *      the received data.
	 * FE = Framing Error. Set when RDRF is set but a zero was found
	 *      in the stop bit position. Indicates misalignment.
	 * PF = Parity Flag. Parity for received data does not match expected Parity Type (PT).
	 */
	if (UART0->S1 & (UART_S1_OR_MASK | UART_S1_NF_MASK |
	UART_S1_FE_MASK | UART_S1_PF_MASK))
	{
		// clear the error flags
		UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK |
		UART0_S1_FE_MASK | UART0_S1_PF_MASK;
		// read the data register to clear RDRF
		ch = UART0->D;
	}

	/* Check if received data register is full (RDRF) */
	if (UART0->S1 & UART0_S1_RDRF_MASK)
	{
		ch = UART0->D;  // Read data, clears RDRF

		/* Echo back to console so user can see what they typed */
		printf("%c", ch);

		/* If there is room in the receiver queue, enqueue the received byte */
		if (cbfifo_length(Q_RX) < cbfifo_capacity(Q_RX))
		{
			cbfifo_enqueue(Q_RX, &ch, 1);
			//Q_Enqueue(&RxQ, ch);
		} else
		{
			// error - queue full.
			// discard character
		}
	}

	/* If transmit interrupt (TDRE-based) is enabled and the transmit data register is empty (TDRE) */
	if ((UART0->C2 & UART0_C2_TIE_MASK) && // transmitter interrupt enabled
			(UART0->S1 & UART0_S1_TDRE_MASK))
	{ // tx buffer empty
		// can send another character
		if (cbfifo_length(Q_TX) > 0)
		{ //!Q_Empty(&TxQ)) {
			char tx_char;
			cbfifo_dequeue(Q_TX, &tx_char, 1);
			UART0->D = tx_char;
		} else
		{
			// queue is empty so disable transmitter interrupt
			UART0->C2 &= ~UART0_C2_TIE_MASK;
		}
	}

	/* Re-enable interrupts to their previous state */
    __set_PRIMASK(pm);
}

void send_string(char *str)
{
	// enqueue string
	while (*str != '\0')
	{ // copy characters up to null terminator
		while (cbfifo_length(Q_TX) == cbfifo_capacity(Q_TX))
		{
		}; // wait for space to open up in the tx queue
		cbfifo_enqueue(Q_TX, str, 1);  // Enqueue a single byte at a time
		str++;
	}
	// start transmitter if it isn't already running
	if (!(UART0->C2 & UART0_C2_TIE_MASK))
	{
		char tx_char; // Single character to store the char to be transmitted
		cbfifo_dequeue(Q_TX, &tx_char, 1); // dequeue one char into the tx buffer
		UART0->D = tx_char;  // Copy tx buffer into the tx data register
		UART0->C2 |= UART0_C2_TIE(1); // Enable the transmit register empty interrupt
	}
}

// *******************************ARM University Program Copyright ARM Ltd 2013*************************************
