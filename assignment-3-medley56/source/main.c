/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "led.h"
#include "tsi.h"
#include "wait.h"
#include "log.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define TSI_POLL_PERIOD_MS 100U  // Period on which to check the TSI for input (ms)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*
 * Execute a traffic light pattern using the LED module (led.h) and the TSI
 * module (tsi.h). The pattern is interruptable by touching the TSI sensor.
 *
 * RED ON for 1000 msec then instantaneous switch to
 * GREEN ON for 1000 msec, then instantaneous switch to
 * YELLOW ON for 1000 msec, then instantaneous switch to RED
 */
void led_traffic_light_pattern();


/*
 * Polls the touch sensor every TSI_POLL_PERIOD_MS for a maximum of duration_ms
 *
 * Parameters
 *   duration_ms  Number of ms to loop for in total, polling TSI every TSI_POLL_PERIOD_MS
 *
 * Returns
 *   The number of ms for which it polled (in increments of TSI_POLL_PERIOD_MS)
 */
uint32_t poll_for_touch(uint32_t duration_ms);


/*******************************************************************************
 * Code
 ******************************************************************************/

/*
 * This function is in main because it requires both LED control and TSI control
 * Leaving this in main leaves the TSI and LED modules fully decoupled.
 */
void led_traffic_light_pattern()
{
	/* Notes on operation:
	 * During touch polling, the LED stays in its previous state unless touch is detected.
	 * The touch poller polls every 100ms (TSI_POLL_PERIOD_MS) up to a maximum of 1000ms.
	 * If touch is detected, the poller runs a custom touch LED sequence and then returns the duration (polled_ms) for which it polled before detecting a touch.
	 * The duration (polled_ms) returned by the poller is used to precisely calculate where to resume the RGY sequence.
	 * After polling for touch, the LEDs may be off (the touch sequence ends by turning LEDs off) so we must re-set the previous color to resume the RGY sequence.
	 */
	static uint32_t polled_ms;  // Time spent polling for touch
	/* RED ON for 1000 msec */
	LOG("CHANGE LED TO RED\r\n");
	led_red();
	polled_ms = poll_for_touch(ONE_THOUSAND_MS);
	led_red();
	wait_ms(ONE_THOUSAND_MS - polled_ms);

	/* GREEN ON for 1000 msec, then instantaneous switch to YELLOW*/
	LOG("CHANGE LED TO GREEN\r\n");
	led_green();
	polled_ms = poll_for_touch(ONE_THOUSAND_MS);
	led_green();
	wait_ms(ONE_THOUSAND_MS - polled_ms);

	/* YELLOW ON for 1000 msec, then instantaneous switch to RED */
	LOG("CHANGE LED TO YELLOW\r\n");
	led_yellow();
	polled_ms = poll_for_touch(ONE_THOUSAND_MS);
	led_yellow();
	wait_ms(ONE_THOUSAND_MS - polled_ms);
	return;
}


uint32_t poll_for_touch(uint32_t duration_ms)
{
	uint32_t elapsed_ms = 0;
	uint32_t touch_value;

	while (elapsed_ms < duration_ms)
	{
		/* Read from the TSI */
		touch_value = tsi_read(TSI_LHS_CHANNEL);
		/* We use the TSI_MAX_THRESHOLD to protect against the case when a thermal effect causes
		 * our TSI_OFFSET to drag the value below the wraparound point, resulting in huge values.
		 * This is unlikely for a well calibrated touch sensor.
		 */
		if (touch_value > TSI_LHS_THRESHOLD && touch_value < TSI_MAX_THRESHOLD)  // LHS touch
		{
			LOG("SLIDER VALUE %u\r\n", touch_value);
			led_touch_pattern_LHS();
			break;
		} else if (touch_value > TSI_RHS_THRESHOLD)  // RHS touch
		{
			LOG("SLIDER VALUE %u\r\n", touch_value);
			led_touch_pattern_RHS();
			break;
		}
		/* If we reach here, the touch_value indicated no touch present */
		wait_ms(TSI_POLL_PERIOD_MS);
		elapsed_ms += TSI_POLL_PERIOD_MS;  // Keep track of elapsed time
	}
	/* The elapsed time is returned to be used to resume the outer loop
	 * up to the precision of a few clock cycles */
	return elapsed_ms;
}


/*!
 * @brief Main function
 */
int main(void)
{
    /* Initialize board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /* Initialize LED control and set GPIO pin state */
    all_led_init();
    /* Run an LED test pattern */
    led_test_pattern();
    /* Initialize the TSI */
    tsi_init();

    while (1)
    {
    	/* Run a touch-interruptable traffic light pattern */
    	led_traffic_light_pattern();
    }
}
