/*
 * led.c
 * 
 * Implementation file for LED control functions
 *
 *  Created on: Sep 25, 2023
 *      Author: Gavin Medley
 */

#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "wait.h"
#include "led.h"
#include "log.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define RED_LED_PIN (18)  // RED is pin 18 on Port B
#define GREEN_LED_PIN (19)  // GREEN is pin 19 on Port B
#define BLUE_LED_PIN (1)  // BLUE is pin 1 on Port D
/* The PIN numbers are also the indices of the corresponding PCOR/PSOR register bits. */
#define RED_LED_SHIFT RED_LED_PIN
#define GREEN_LED_SHIFT GREEN_LED_PIN
#define BLUE_LED_SHIFT BLUE_LED_PIN
#define MASK(x) (1UL << (x))  // Adapted from Alexander Dean book, p. 40

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

/*************************************************************************************
 * Internal functions that do specific LED operations without side effects
 * Note: These are not exposed via the led.h API
 *************************************************************************************/

static void red_led_on()
{
	GPIOB->PCOR = MASK(RED_LED_SHIFT);
}


static void green_led_on()
{
	GPIOB->PCOR = MASK(GREEN_LED_SHIFT);
}


static void blue_led_on()
{
	GPIOD->PCOR = MASK(BLUE_LED_SHIFT);
}


static void red_led_off()
{
	GPIOB->PSOR = MASK(RED_LED_SHIFT);
}


static void green_led_off()
{
	GPIOB->PSOR = MASK(GREEN_LED_SHIFT);
}


static void blue_led_off()
{
	GPIOD->PSOR = MASK(BLUE_LED_SHIFT);
}

/******************************
 * LED Initialization function
 ******************************/
void all_led_init()
{
	/* Enable clock signal to PORTB and PORTD */
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	/* Clear MUX setting for Pin Control Register (PCR) */
	PORTB->PCR[RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	/* Set PCR to GPIO MUX mode */
	PORTB->PCR[RED_LED_PIN] |= PORT_PCR_MUX(1);
	/* Same for green and blue pins */
	PORTB->PCR[GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_PIN] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_PIN] |= PORT_PCR_MUX(1);
	/* Set Port Data Direction Registers to output mode */
	GPIOB->PDDR |= MASK(RED_LED_PIN) | MASK(GREEN_LED_PIN);
	GPIOD->PDDR |= MASK(BLUE_LED_PIN);
	/* Start with all LEDs off */
	led_off();
}

/*************************************************************************************
 * Interface functions that do whatever is required to make the LED the desired color
 *************************************************************************************/

void led_red()
{
	red_led_on();
	green_led_off();
	blue_led_off();
	return;
}


void led_green()
{
	red_led_off();
	green_led_on();
	blue_led_off();
	return;
}


void led_blue()
{
	red_led_off();
	green_led_off();
	blue_led_on();
	return;
}


void led_yellow()
{
	red_led_on();
	green_led_on();
	blue_led_off();
	return;
}


void led_white()
{
	red_led_on();
	green_led_on();
	blue_led_on();
	return;
}


void led_off()
{
	red_led_off();
	green_led_off();
	blue_led_off();
}

/*************************************************************************************
 * LED patterns
 *************************************************************************************/

void led_test_pattern()
{
	/* WHITE for 1000 msec, OFF for 500 msec */
	LOG("CHANGE LED TO WHITE\r\n");
    led_white();
    wait_ms(ONE_THOUSAND_MS);
    LOG("CHANGE LED TO OFF\r\n");
    led_off();
    wait_ms(FIVE_HUNDRED_MS);
	/* BLUE for 1000 msec, OFF for 500 msec */
    LOG("CHANGE LED TO BLUE\r\n");
    led_blue();
    wait_ms(ONE_THOUSAND_MS);
    LOG("CHANGE LED TO OFF\r\n");
    led_off();
    wait_ms(FIVE_HUNDRED_MS);
	/* BLUE for 1000 msec, OFF for 500 msec */
    LOG("CHANGE LED TO BLUE\r\n");
    led_blue();
	wait_ms(ONE_THOUSAND_MS);
	LOG("CHANGE LED TO OFF\r\n");
	led_off();
	wait_ms(FIVE_HUNDRED_MS);
	/* WHITE for 1000 msec, OFF for 500 msec */
	LOG("CHANGE LED TO WHITE\r\n");
	led_white();
	wait_ms(ONE_THOUSAND_MS);
	LOG("CHANGE LED TO OFF\r\n");
	led_off();
	wait_ms(FIVE_HUNDRED_MS);
	return;
}


void led_touch_pattern_RHS()
{
	for (int i = 0; i < 3; i++)
	{
		LOG("CHANGE LED TO BLUE\r\n");
		led_blue();
		wait_ms(ONE_THOUSAND_MS);
		LOG("CHANGE LED TO OFF\r\n");
		led_off();
		wait_ms(FIVE_HUNDRED_MS);
	}
	return;
}


void led_touch_pattern_LHS()
{
	for (int i = 0; i < 3; i++)
	{
		LOG("CHANGE LED TO WHITE\r\n");
		led_white();
		wait_ms(ONE_THOUSAND_MS);
		LOG("CHANGE LED TO OFF\r\n");
		led_off();
		wait_ms(FIVE_HUNDRED_MS);
	}
	return;
}
