/*
 * pidctl.c
 *
 * Module for controlling a motor with a rudimentary PID control algorithm.
 *
 *  Created on: Dec 10, 2023
 *      Author: gmedley
 */

#include "pidctl.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "timing.h"
#include "encoder.h"
#include "scmd.h"
#include "led.h"
#include "tpm.h"

#define K_P (40)
#define K_I (4)
#define K_D (5)
#define PID_SCALE (110000)  // This scales our PID calculation so we can stick to integer math
#define TOL (50)
#define T (1U)  // Sampling period, i.e. number of ticks between encoder position samples
#define MAX_VALID_ENCODER COUNTS_PER_ROTATION / 2
#define MIN_VALID_ENCODER -MAX_VALID_ENCODER

int32_t set_point = 0;  // global extern written by external modules
bool tolerance_met = true;
int32_t position, error, integral = 0, derivative = 0, prev_error = 0, prev_set_point = 0;
uint8_t drive;  // Drive value
uint32_t n = 0;  // Number of iterations
uint32_t n_max = 100;  // Max number of iterations before giving up
int32_t pid_out;  // Unscaled PID value (before converting to a SCMD drive value)

void update_pid(void)
{
    /* Get current encoder position */
    position = encoder_position();
    error = set_point - position;

    /* Check to see if we have reached the correct location and stop if so */
    if (abs(error) < TOL)
    {
        if (!tolerance_met)
        {
            printf("Tolerance met. error: %d (press Enter)\r\n", error);
            led_control_hex(0x00ff00);
        }
        scmd_ma_set_drive(SCMD_ZERO_DRIVE);
        tolerance_met = true;
        return;
    }

    /* Prevents PID values from previous motor path from polluting the current execution */
    if (set_point != prev_set_point)
    {
        //printf("New motor set point: %d\r\n", set_point);
        n = 0;  // Number of iterations in attempt to reach the set point
        integral = 0;
        derivative = 0;
        prev_error = error;
        prev_set_point = set_point;
        tolerance_met = false;
    }

    if (tolerance_met)
    {
        return;
    }

    if (n >= n_max)
    {
        /* We were unable to reach the desired position in n_max iterations */
        if (!tolerance_met)
        {
            printf("Unable to reach set point %d in %d iterations. Reached %d (press Enter)\r\n", set_point, n_max, position);
            led_control_hex(0x0000ff); // blue for failed to meet tolerance
        }
        scmd_ma_set_drive(SCMD_ZERO_DRIVE);  // Don't move motor
        tolerance_met = true;
        return;
    }

    if (safe_mode)
    {
        led_control_hex(0xff0000);  // red, going to safe
    } else
    {
        led_control_hex(0xffffff); // white for moving
    }

    //printf("P: %d\r\n", error);
    integral += error * T;  // Current error * sampling period
    //printf("I: %d\r\n", integral);
    derivative = (error - prev_error) / T;
    //printf("D: %d\r\n", derivative);
    pid_out = ((K_P * error) + (K_I * integral) + (K_D * derivative));
    //printf("PID Output: %d\r\n", pid_out);

    /* If we are out of bounds, immediately set max drive to get us back to bounds */
    if (position > MAX_VALID_ENCODER || pid_out < 0)
    {
        drive = 0x00;  // Full reverse
    } else if (position < MIN_VALID_ENCODER || pid_out > PID_SCALE)
    {
        drive = 0xFF;  // Full forward
    } else {
        drive = (uint8_t)(SCMD_ZERO_DRIVE + ((127 * pid_out) / PID_SCALE));  // Scale to between 0 and 0xFF
    }

    //printf("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", set_point, position, error, integral, derivative, K_P * error, K_I * integral, K_D * derivative, pid_out, drive);
    /* Set the new motor drive value */
    scmd_ma_set_drive(drive);
    n++;
}
