/*
 * tsi.h
 *
 *  Created on: Sep 26, 2023
 *      Author: Gavin Medley
 */

#ifndef TSI_H_
#define TSI_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TSI_IRQn TSI0_IRQn  // So we can easily disable the correct interrupt externally

/* Global variable for indicating the TSI touch status
 * If tsi_touched is true, it means the TSI IRQ handler has executed a
 * and ODD number of times. If tsi_touched is false, the IRQ handler
 * has executed an EVEN number of times.
 * */
extern bool tsi_touched;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*
 * Initialize the Touch Sensor Input interface
 */
void tsi_init();

/*
 * Start a TSI scan.
 */
void tsi_start_scan();

#endif /* TSI_H_ */
