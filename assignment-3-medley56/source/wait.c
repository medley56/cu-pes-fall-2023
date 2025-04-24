/*
 * wait.c
 *
 *  Created on: Sep 27, 2023
 *      Author: Gavin Medley
 */
#include "fsl_debug_console.h"
#include "wait.h"
#include "log.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define WAIT_LOOPS_PER_MS 5400  // Empirically derived number of wait loop iterations per ms

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void wait_ms(uint32_t ms)
{
	if (ms)
	{
		LOG("START TIMER %u\r\n", ms);
	}
	/* The loop itself has some overhead. The operations ms-- and the comparison ms != 0 will each take some
	 * number of cycles to execute. This means that even if the NOP instruction is one cycle, the whole loop
	 * iteration might be more than that due to these extra operations.
	 */
	uint32_t nloops = ms * WAIT_LOOPS_PER_MS;
	while (nloops-- != 0)
	{
		/* This is a directive to include assembly language in C code

		volatile tells the C compiler not to optimize anything about it or reorder it.
		NOP is a "no-operation" assembly instruction. It introduces a single cycle delay
		*/
		__asm volatile("NOP");
	}
	return;
}
