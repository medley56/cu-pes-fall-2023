/*
 * File gives the implementation of the static profiler functions.
 *
 *  Created on: Aug 2, 2023
 *      Author: lpandit
 */

#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "static_profiler.h"

/* Declarations of the counter and profiling flag */
uint32_t ISHAProcessMessageBlockCount, ISHAPadMessageCount, ISHAResetCount, ISHAInputCount, ISHAResultCount;
bool static_profiling_on ;

/*
 * Turns the static profiler on.
 */
void static_profile_on(void)
{
	static_profiling_on = true;
}

/*
 * Turns the static profiler off.
 */
void static_profile_off(void)
{
	static_profiling_on = false;
}

/*
 *  Prints the summary of the profiling.
 */
void print_static_profiler_summary(void)
{
	PRINTF("Static profiling results:\r\n");
	PRINTF("Function: ISHAProcessMessageBlock    Call count: %u\r\n", ISHAProcessMessageBlockCount);
	PRINTF("Function: ISHAPadMessage             Call count: %u\r\n", ISHAPadMessageCount);
	PRINTF("Function: ISHAReset                  Call count: %u\r\n", ISHAResetCount);
	PRINTF("Function: ISHAInput                  Call count: %u\r\n", ISHAInputCount);
	PRINTF("Function: ISHAResult                 Call count: %u\r\n", ISHAResultCount);
	PRINTF("End of static profiling results\r\n");
}
