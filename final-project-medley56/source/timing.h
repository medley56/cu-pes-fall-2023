/**
 * @file timing.h
 * @brief Header file for timing-related functions.
 *
 * @date 2023-10-08
 * @author Gavin Medley
 */

#ifndef TIMING_H_
#define TIMING_H_

#include <stdint.h>

#define CLK_FREQ 24000000U  // Base clock frequency is 48MHz but this number depends on MCG clock configuration
#define SEC_FRAC 16U  // Fraction of a second to count (16 => 1/16th)
#define ONE_SECOND (SEC_FRAC * 1)  // Number of ticks in 1s

/* Tick time type. Represents a number of ticks where 1 tick is 1/SEC_FRAC seconds */
typedef uint32_t ticktime_t;

/**
 * @brief Initialize the timing system
 */
void systick_init();

/**
 * @brief Return time since startup
 */
ticktime_t now(); // in sixteenths of a second

/**
 * @brief Reset the timer to zero. Does not affect now() values.
 */
void reset_global_timer();

/**
 * @brief Return ticks since the last call to reset_global_timer().
 */
ticktime_t get_global_timer();

/**
 * @brief Blocking delay, measured in ticks.
 *
 * Causes the program to wait for a specified number of ticks. Each tick is defined
 * as 1/SEC_FRAC seconds.
 *
 * @param ticks Number of ticks to wait.
 */
void delay(uint32_t ticks);

#endif /* TIMING_H_ */
