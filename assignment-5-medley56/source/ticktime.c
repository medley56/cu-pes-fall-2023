/**
 * @file    ticktime.c
 * @brief   Code to map SysTick timer into msec counter
 * @author  Howdy Pierce, howdy.pierce@colorado.edu
 */

#include "MKL25Z4.h"
#include "core_cm0plus.h"
#include "ticktime.h"
#include "fsl_debug_console.h"
#include "pc_profiler.h"

#define MS_PER_S 1000
#define TICKS_PER_SECOND 10000
static volatile ticktime_t g_now = 0;
static ticktime_t g_timer = 0;

uint32_t prev_pc;

void init_ticktime(void) {
	// set control & status register to use 48 MHz/16 = 3 MHz
	// clock. Then interrupt 10000 times per second
	SysTick->LOAD = (48000000 / 16) / (TICKS_PER_SECOND);
	NVIC_SetPriority(SysTick_IRQn, 3);
	NVIC_ClearPendingIRQ(SysTick_IRQn);
	NVIC_EnableIRQ(SysTick_IRQn);
	SysTick->VAL = 0;
	SysTick->CTRL &= ~(SysTick_CTRL_CLKSOURCE_Msk);
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	g_now = 0;
	g_timer = 0;
}

void SysTick_Handler(void) {
	// Store PC value
	asm("ldr %0, [sp, #32]" // Load the value from memory address (sp + 24) into 'prev_pc'
			: "=r"(prev_pc)// Store value in global variable
	);
	//PRINTF("TICK!\r\n");
	g_now++;  // Increment tick counter
	if (pc_profiling_on) {
		//PRINTF("Previous PC address: %u\r\n", prev_pc);
		pc_profile_check(&prev_pc);
	}
}

ticktime_t now(void) {
	// [ticks] * [ms/s] / [ticks/s] = [ms]
	return (g_now * MS_PER_S) / TICKS_PER_SECOND;
}

void reset_timer(void) {
	g_timer = g_now;
}

ticktime_t get_timer(void) {
	// [ticks] * [ms/s] / [ticks/s] = [ms]
	return ((g_now - g_timer) * MS_PER_S) / TICKS_PER_SECOND;
}
