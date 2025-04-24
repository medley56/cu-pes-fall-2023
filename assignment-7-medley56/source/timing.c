/*
 * timing.c
 *
 *  Created on: Oct 8, 2023
 *      Author: Gavin Medley
 */

#include "log.h"
#include "MKL25Z4.h"
#include "core_cm0plus.h"
#include "fsl_debug_console.h"
#include "dac.h"
#include "adc.h"
#include "dma.h"
#include "timing.h"


#define CLK_FREQ 48000000U  // 48MHz core clock base frequency
#define EXT_CLK_PRESCALE 16U  // Prescale value for external clock (from ARMv6-M manual)
#define RELOAD_VALUE ((N_SECONDS * CLK_FREQ / EXT_CLK_PRESCALE) - 1)  // Reload value (1 tick worth of cycles)
#define SYSTICK_IRQ_PRIORITY 3U  // SysTick IRQ priority
/*
 * Note: with SEC_FRAC (in timing.h) set to 16 (1tick = 1/16s)
 * storing ticks in a uint32_t the MCU can count for
 * (1/16[1/sec] * 2^32[ticks]) / 3600[secs/hr] / 24[hrs/day] / 365[days/yr] = 8.5yrs
 */

static ticktime_t start_time;
static ticktime_t subseconds_since_startup;  // Count of subseconds since system boot up to (2^32) / 16 / 3600 / 24 = ~3106 days
uint8_t mode = SQUARE_MODE;


void SysTick_Handler()
{
	/* Save interrupt mask state and disable interrupts */
	/* Increment internal time counter */
	subseconds_since_startup++;

	Summarize_Waveform();

	if (mode == SQUARE_MODE)
	{
		LOG("Running SQUARE waveform");
		waveform_to_samples(SQUARE_MODE);
		mode = SINE_MODE;
	} else if (mode == SINE_MODE)
	{
		LOG("Running SINE waveform");
		waveform_to_samples(SINE_MODE);
		mode = TRIANGLE_MODE;
	}
	else
	{
		LOG("Running TRIANGLE waveform");
		waveform_to_samples(TRIANGLE_MODE);
		mode = SQUARE_MODE;
	}

	/* Start playback with new waveform buffer */
	Init_DMA_For_Playback(sample_buffer, num_samples);
	Start_DMA_Playback();

	/* Now that the waveform is playing start sampling with the ADC on TPM1 */
	Reset_ADC_Sampling();
	NVIC_EnableIRQ(TPM1_IRQn);
}

void systick_init()
{
	LOG("Initializing SysTick...");
	SysTick->LOAD = RELOAD_VALUE;  // Set the reload value to the max value that the 24bit current value register (CVR) can hold (results in 1Hz interrupts)
	SysTick->VAL = 0;  // Force the counter to set its reload value

	/* CLKSOURCE = 0 (default) uses external clock (core clock prescaled by 16), 1 uses core clock
	 * ENABLE = 1 enables the counter
	 * TICKINT = 1 turns on counter interrupt routine */
	SysTick->CTRL = SysTick_CTRL_TICKINT_Msk |  // Enable systick interrupt to count global time since startup
			        SysTick_CTRL_ENABLE_Msk;  // Enable the systick counter

	NVIC_SetPriority(SysTick_IRQn, SYSTICK_IRQ_PRIORITY);
	NVIC_ClearPendingIRQ(SysTick_IRQn);
	NVIC_EnableIRQ(SysTick_IRQn);
	LOG("Done.");
}

ticktime_t now()  // Ticks since startup
{
	return subseconds_since_startup;
}

void reset_global_timer() // Resets timer to 0; doesn't affect now() values
{
	start_time = now();
}

ticktime_t get_global_timer() // Returns ticks since the last call to reset_timer()
{
	return now() - start_time;
}

void delay(uint32_t ticks)
{
	uint32_t end = now() + ticks;
	while (now() < end)
	{
		/* This is a directive to include assembly language in C code.
		It has been included to ensure this function works correctly even if -O0 is not set.

		volatile tells the C compiler not to optimize anything about it or reorder it.
		NOP is a "no-operation" assembly instruction. It introduces a single cycle delay
		*/
		__asm volatile("NOP");
	}
}

