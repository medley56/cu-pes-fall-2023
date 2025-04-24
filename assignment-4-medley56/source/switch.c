/*
 * switch.c
 *
 *  Created on: Oct 15, 2023
 *      Author: Gavin Medley
 */

#include <stdbool.h>
#include "MKL25Z4.h"
#include "core_cm0plus.h"
#include "fsl_debug_console.h"
#include "timing.h"
#include "log.h"
#include "switch.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SWITCH_PORT PORTD  // Only PORTA and PORTD support interrupt generation
#define SWITCH_GPIO GPIOD
#define SWITCH_SCGC_PORT_MASK SIM_SCGC5_PORTD_MASK
#define SWITCH_IRQC_MODE 11U  // Either edge (1011)
#define SWITCH_IRQ_PRIORITY 2U  // 2 out of 3
#define SWITCH_PIN 2U
#define SWITCH_CLOSED false
#define SWITCH_OPEN true

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
bool switch_changed = false;

/*******************************************************************************
 * Code
 ******************************************************************************/
void switch_init()
{
	LOG("Initializing SWITCH interrupt pin...");

	/* Send clock to relevant PORT */
	SIM->SCGC5 |= SWITCH_SCGC_PORT_MASK;

	/* Set Pin Control Register to:
	 * Set MUX mode to GPIO
	 * Enable the pull up resistor (PE)
	 * Select pull up (PS)
	 * Set IRQC mode to both edges
	 */
	/* SWITCH PIN */
	LOG("Configuring switch GPIO");
	SWITCH_PORT->PCR[SWITCH_PIN] = PORT_PCR_MUX(1) |
								   PORT_PCR_PE_MASK |
								   PORT_PCR_PS_MASK; // Pull UP

	/* Set GPIO direction to input (PDIR register bit = 0) */
	LOG("Setting switch pin direction to input");
	SWITCH_GPIO->PDDR &= ~(1U << SWITCH_PIN);

	LOG("Enabling IRQ");
	/* We do this separately from the rest of the PCR config so that the
	 * interrupt doesn't fire immediately as the mux mode is being set. */
	SWITCH_PORT->PCR[SWITCH_PIN] |= PORT_PCR_IRQC(SWITCH_IRQC_MODE);

	/* Configure NVIC */
	LOG("Enabling switch interrupt");
	NVIC_SetPriority(SWITCH_IRQn, SWITCH_IRQ_PRIORITY);  // Set priority
	NVIC_ClearPendingIRQ(SWITCH_IRQn);  // Clear any pending requests
	NVIC_EnableIRQ(SWITCH_IRQn);  // Enable detection of events
	LOG("Done.");
}


bool debounce_switch(uint32_t pin)
{
	uint32_t state = 0b01010101010101010101010101010101U;  // Initial series of values
	const uint32_t mask = 0b00000000000000001111111111111111U;  // Number of times we check the value
	/* Note: I apparently have an extremely noisy switch. Anything less than 16 consecutive
	 * ones or zeros leads to false readings, possibly because I don't have a delay in the debouncer. */
	while (1)
	{
		state = ((state << 1) | ((SWITCH_GPIO->PDIR & (1U << pin)) >> pin)) & mask;
		if (state == mask)  // Series of ones
		{
			return SWITCH_OPEN;
		}
		if (state == 0)  // Series of zeros
		{
			return SWITCH_CLOSED;
		}
	}
}

void PORTD_IRQHandler(void)
{
	/* This interrupt fires on every rising or falling edge
	 * With a noisy button it may fire many times so we disable the interrupt
	 * while we debounce. Hopefully this prevents multiple interrupts from queuing
	 * up while we're handling the initial rising/falling edge.
	 */
	/* Save interrupt mask state and disable interrupts */
	uint32_t pm = __get_PRIMASK();
	__disable_irq();

	/* This effectively introduces a slight delay while we wait for the switch state to stabilize */
	debounce_switch(SWITCH_PIN);

	/* Indicate globally that the switch has changed state */
	switch_changed = true;
	// LOG("[%u] SWITCH CHANGED", now());

	/* Clear SWITCH port interrupt status flag */
	SWITCH_PORT->ISFR = 0xFFFFFFFFU;

	/* Re-enable interrupts to their previous state */
	__set_PRIMASK(pm);
}
