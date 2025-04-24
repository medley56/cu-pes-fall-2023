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
	// Enter your code here
}

/*
 * Turns the static profiler off.
 */
void static_profile_off(void)
{
	// Enter your code here
}

/*
 *  Prints the summary of the profiling.
 */
void print_static_profiler_summary(void)
{
	// Enter your code here
}
