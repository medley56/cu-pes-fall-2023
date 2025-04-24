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

#define SET_LED_WHITE led_control_hex(0xFFFFFFU)  // 0xFFFFFF
#define SET_LED_OFF led_control_hex(0x000000)  // 0x000000
#define SET_LED_RED led_control_hex(0xFF0000)  // 0xFF0000
#define SET_LED_BLUE led_control_hex(0x0000FF)  // 0x0000FF
#define SET_LED_GREEN led_control_hex(0x00FF00)  // 0x00FF00
#define SET_LED_YELLOW led_control_hex(0xFFFF00)  // 0xFFFF00

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*
 * Initialize all LED GPIO pins
 *
 * Adapted from:
 * Embedded Systems Fundamentals with Arm Cortex-M based Microcontrollers
 * by Alexander G. Dean
 */
void led_init();

/*
 * Control tricolor LED using PWM. Input is the standard hex triplet for RGB where
 * each value can be [0,255] inclusive. 255 is full brightness, 0 is off.
 *
 * Parameters
 *   r  Red duty cycle [0, 255]
 *   g  Green duty cycle [0, 255]
 *   b  Blue duty cycle [0, 255]
 */
void led_control(uint8_t r, uint8_t g, uint8_t b);

/*
 * Control tricolor LED using PWM via a single hex value.
 *
 * Parameters
 *   rgb  32 bit hex value representing --RRGGBB values.
 */
void led_control_hex(uint32_t rgb);

#endif /* LED_H_ */
