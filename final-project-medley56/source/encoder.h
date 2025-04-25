/*
 * encoder.h
 *
 *  Created on: Dec 10, 2023
 *      Author: gmedley
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include <stdint.h>

#define COUNTS_PER_ROTATION (6900)  // Encoder counts per full rotation

/*
 * Initialize encoder interrupt pins to register encoder events
 */
void encoder_interrupt_init(void);


/*
 * @brief Get the current encoder position value
 *
 * @return Current position (0 is home)
 */
int32_t encoder_position(void);


#endif /* ENCODER_H_ */
