/*
 * profiler.h
 *
 *  Created on: May 24, 2023
 *      Author: lpandit
 *
 *
 */
#include "stdbool.h"
#include "stdint.h"

#ifndef _PC_PROFILER_H_
#define _PC_PROFILER_H_

/* Flag to indicate if pc profiling is on */
extern bool pc_profiling_on;

/*
 * Check if the program count pc belongs to one of the isha functions.
 *
 * Parameters:
 *   pc   - Program Counter
 */
void pc_profile_check(uint32_t *pc);

/*
 * Turn the pc profiler on for call counting.
 */
void pc_profile_on(void);

/*
 * Turn the pc profiler off for call counting.
 */
void pc_profile_off(void);


/*
 * Print a summary of profiler calls for following functions.
 *
 */
void print_pc_profiler_summary(void);
#endif
