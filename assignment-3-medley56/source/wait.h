/*
 * wait.h
 *
 *  Created on: Sep 27, 2023
 *      Author: Gavin Medley
 */

#ifndef WAIT_H_
#define WAIT_H_

#include <stdint.h>

#define FIVE_HUNDRED_MS 500U
#define ONE_THOUSAND_MS 1000U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*
 * Wait for (approximately) ms milliseconds
 *
 * Parameters
 *   ms  Number of milliseconds to wait.
 */
void wait_ms(uint32_t ms);

#endif /* WAIT_H_ */
