/*
 * pidctl.h
 *
 * Module that implements a PID motor control algorithm for a DC motor with encoders.
 *
 *  Created on: Dec 10, 2023
 *      Author: gmedley
 */

#ifndef PIDCTL_H_
#define PIDCTL_H_

#include <stdint.h>
#include <stdbool.h>

#define MTR_SAFE_POSITION (0)

extern int32_t set_point;  // Target motor position
extern bool tolerance_met;  // Indicates the motor operation is complete

/*
 * @brief Update the PID drive value of the motor.
 *
 * This updates the current drive value based on the current PID state and the set_point,
 * which is typically set externally
 */
void update_pid(void);

/*
 * @brief Drive the DC motor to a specific position.
 *
 * This uses a PID control method to drive the motor to a specific location.
 *
 * @param encoder_position The location to drive the motor to (signed)
 */
void home_to_position(int32_t encoder_position);

#endif /* PIDCTL_H_ */
