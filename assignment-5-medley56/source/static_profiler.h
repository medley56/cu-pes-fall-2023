/*
 * Header file for the static profiler functions.
 *
 *  Created on: Aug 2, 2023
 *      Author: lpandit
 */

#include "stdint.h"
#include "stdbool.h"

#ifndef STATIC_PROFILER_H_
#define STATIC_PROFILER_H_


/* Count values for number of times each function is called when the profiler is turned on. */
extern uint32_t ISHAProcessMessageBlockCount, ISHAPadMessageCount, ISHAResetCount, ISHAInputCount, ISHAResultCount;

/* Flag to indicate if the profiler is on */
extern bool static_profiling_on ;

/*
 * Turns the static profiling on.
 *
 */
void static_profile_on(void);

/*
 *  Turns the static profiling off.
 */
void static_profile_off(void);

/*
 *  Prints a summary of the static profiling.
 */
void print_static_profiler_summary(void);


#endif /* STATIC_PROFILER_H_ */
