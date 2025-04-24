/*
 * timing.h
 *
 *  Created on: Oct 8, 2023
 *      Author: Gavin Medley
 */

#ifndef TIMING_H_
#define TIMING_H_

#include <stdint.h>

#define SEC_FRAC 16U  // Fraction of a second to count (16 => 1/16th)

/* Tick time type. Represents a number of ticks where 1 tick is 1/SEC_FRAC seconds */
typedef uint32_t ticktime_t;

/*
 * Initialize the timing system
 */
void systick_init();

/*
 * Return time since startup
 */
ticktime_t now(); // in sixteenths of a second

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
 *   ticks  Number of ticks to wait. 1 tick is defined as 1/SEC_FRAC seconds.
 */
void delay(uint32_t ticks);

#endif /* TIMING_H_ */
