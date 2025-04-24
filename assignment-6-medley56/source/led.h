/**
 * @file led.h
 * @brief Header file for LED control functions.
 *
 * This header file contains an API for controlling LEDs. The code is
 * largely adapted from "Embedded Systems Fundamentals with Arm Cortex-M based Microcontrollers"
 * by Alexander G. Dean, Chapter 2.
 *
 * @date 2023-09-23
 * @author Gavin Medley
 */

#ifndef LED_H_
#define LED_H_

#include <stdint.h>

#define SET_LED_WHITE led_control_hex(0xFFFFFFU)  // 0xFFFFFF
#define SET_LED_OFF led_control_hex(0x000000)  // 0x000000
#define SET_LED_RED led_control_hex(0xFF0000)  // 0xFF0000
#define SET_LED_BLUE led_control_hex(0x0000FF)  // 0x0000FF
#define SET_LED_GREEN led_control_hex(0x00FF00)  // 0x00FF00
#define SET_LED_YELLOW led_control_hex(0xFFFF00)  // 0xFFFF00

/**
 * @brief Initialize all RGB LED GPIO pins
 *
 * Adapted from:
 * Embedded Systems Fundamentals with Arm Cortex-M based Microcontrollers
 * by Alexander G. Dean
 */
void led_init();

/**
 * @brief Control tricolor LED using PWM.
 *
 * Sets the duty cycle for a tricolor LED using standard RGB hex triplet values, where each color
 * component (red, green, blue) can range from 0 (off) to 255 (full brightness).
 *
 * @param r Red duty cycle, in the range [0, 255].
 * @param g Green duty cycle, in the range [0, 255].
 * @param b Blue duty cycle, in the range [0, 255].
 */
void led_control(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Control tricolor LED using PWM via a single hex value.
 *
 * Sets the RGB components for a tricolor LED using a single 32-bit hex value. This hex value
 * represents the colors in the format --RRGGBB (the two leading digits are ignored).
 *
 * @param rgb 32-bit hex value representing the colors in 0xRRGGBB format.
 */
void led_control_hex(uint32_t rgb);

#endif /* LED_H_ */
