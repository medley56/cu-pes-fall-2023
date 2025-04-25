/*
 * pt.h
 *
 * Module for initializing and using a phototransistor as a light sensor.
 *
 *  Created on: Dec 10, 2023
 *      Author: gmedley
 */

#ifndef PT_H_
#define PT_H_

#include <stdint.h>


/*
 * @brief Initialize ADC to read phototransistor
 */
void pt_init(void);


/*
 * @brief Read current PT value
 *
 * @return Measurement of voltage at emitter end of PT
 */
uint16_t pt_read(void);

#endif /* PT_H_ */
