/*
 * tsi.h
 *
 *  Created on: Sep 26, 2023
 *      Author: Gavin Medley
 */

#ifndef TSI_H_
#define TSI_H_

#define TSI_MAX_THRESHOLD 1000000U  // Maximum realistic value for the touch value to prevent wraparound errors
#define TSI_LHS_THRESHOLD 300U  // Minimum value to register a LHS touch
#define TSI_RHS_THRESHOLD 10U  // Minimum value to register a RHS touch
#define TSI_LHS_CHANNEL 9U  // Channel for the LHS of the TSI
#define TSI_RHS_CHANNEL 9U  // Channel for the RHS of the TSI
#define TSI_OFFSET 281U  // Offset to subtract from TSI count data. Offset is calibrated that no touch is between 0 and 10.
#define TSI_NSCN 16U  // Scans 17 times (zero-indexed)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*
 * Initialize the Touch Sensor Input interface
 */
void tsi_init();

/*
 * Read data from the TSI (must be initialized)
 *
 * Parameters
 *   tsich  Channel to read from. Must be 9 or 10.
 *
 * Returns
 *   Number of oscillations measured over the configured number of scans
 */
uint32_t tsi_read(uint32_t tsich);

#endif /* TSI_H_ */
