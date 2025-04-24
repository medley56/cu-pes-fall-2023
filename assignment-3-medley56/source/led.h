/*
 * led.h
 *
 * Header file for LED control functions. 
 * 
 * This code was largely adapted from 
 * Embedded Systems Fundamentals with Arm Cortex-M based Microcontrollers
 * by Alexander G. Dean
 * Chapter 2
 * 
 *  Created on: Sep 25, 2023
 *      Author: Gavin Medley
 */

#ifndef LED_H_
#define LED_H_

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*
 * Change LED color to RED
 */
void led_red();

/*
 * Change LED to GREEN
 */
void led_green();

/*
 * Change LED to BLUE
 */
void led_blue();

/*
 * Change LED to YELLOW
 */
void led_yellow();

/*
 * Change LED to WHITE (turn all LEDs on)
 */
void led_white();

/*
 * Turn all LEDs to OFF
 */
void led_off();

/*
 * Initialize all LED GPIO pins
 *
 * Adapted from:
 * Embedded Systems Fundamentals with Arm Cortex-M based Microcontrollers
 * by Alexander G. Dean
 */
void all_led_init();

/*
 * Execute a startup test pattern
 *
 * WHITE for 1000 msec, OFF for 500 msec,
 * BLUE for 1000 msec, OFF for 500 msec,
 * BLUE for 1000 msec, OFF for 500 msec
 * WHITE for 1000 msec, OFF for 500 msec
 */
void led_test_pattern();

/*
 * Execute LED pattern for LHS touch
 *
 * WHITE for 1000 msec, OFF for 500 msec,
 * WHITE for 1000 msec, OFF for 500 msec,
 * WHITE for 1000 msec, OFF for 500 msec
 */
void led_touch_pattern_LHS();

/*
 * Execute LED pattern for RHS touch
 *
 * BLUE for 1000 msec, OFF for 500 msec,
 * BLUE for 1000 msec, OFF for 500 msec,
 * BLUE for 1000 msec, OFF for 500 msec
 */
void led_touch_pattern_RHS();

#endif /* LED_H_ */
