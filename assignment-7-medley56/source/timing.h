/*
 * timing.h
 *
 *  Created on: Oct 8, 2023
 *      Author: Gavin Medley
 */

#ifndef TIMING_H_
#define TIMING_H_

#include <stdint.h>

#define N_SECONDS (2U)  // Number of seconds to count for each tick

/* Tick time type. Represents a number of ticks where 1 tick is 1/SEC_FRAC seconds */
typedef uint32_t ticktime_t;

/*
 * Initialize the timing system
 */
void systick_init();

/*
 * Return time since startup
 */
ticktime_t now(); // in ticks

/*
 * Reset the timer to zero. Does not affect now() values.
 */
void reset_global_timer();

/*
 * Returns ticks since the last call to reset_global_timer().
 */
ticktime_t get_global_timer();

/*
 * Blocking delay, in ticks
 *
 * Parameters
 *   ticks  Number of ticks to wait. 1 tick is defined as N_SECONDS seconds.
 */
void delay(uint32_t ticks);

#endif /* TIMING_H_ */
