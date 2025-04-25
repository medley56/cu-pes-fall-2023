/*
 * PBKDF1.c
 *
 * Application entry point, and test timing.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pc_profiler.h>
#include <string.h>

#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"

#include "pbkdf1.h"
#include "pbkdf1_test.h"
#include "ticktime.h"

#include "static_profiler.h"

/*
 * Times a single call to the pbkdf1 function, and prints
 * the resulting duration.
 *
 * Parameters:
 *     print_time - Boolean flag to indicate if the time duration should be printed.
 */
static void time_pbkdf1(bool print_time) {
	const char *pass = "Boulder";
	const char *salt = "Buffaloes";
	int iterations = 4096;
	size_t dk_len = 20;
	int passlen, saltlen;
	uint8_t act_result[512];
	uint8_t exp_result[512];

	const char *exp_result_hex = "E9C8B4E075D3BB7652204AD6CBBE19B44051EFB4";

	ticktime_t duration = 0;

	assert(dk_len <= sizeof(act_result));

	hexstr_to_bytes(exp_result, exp_result_hex, dk_len);
	passlen = strlen(pass);
	saltlen = strlen(salt);

	reset_timer();
	error_t err = pbkdf1((const uint8_t*) pass, passlen, (const uint8_t*) salt,
			saltlen, iterations, act_result, dk_len);
	duration = get_timer();

	if ((err == NO_ERROR) && cmp_bin(act_result, exp_result, dk_len)) {
		if (print_time) {
			PRINTF("%s: %u iterations took %u msec\r\n", __FUNCTION__,
					iterations, duration);
		} else {
			PRINTF("%s: %u iterations complete\r\n", __FUNCTION__, iterations,
					duration);
		}
	} else {
		if (print_time) {
			PRINTF("FAILURE on timed test duration=%u msec\r\n", duration);
		} else {
			PRINTF("FAILURE on tests.\r\n", duration);
		}
	}
}

/*
 * Run all the validity checks; exit on failure
 */
static void run_tests() {
	bool success = true;

	success &= test_isha();
	success &= test_pbkdf1();

	if (success)
		return;

	PRINTF("TEST FAILURES EXIST ... exiting\r\n");

	exit(-1);
}

/*
 * Application entry point.
 */
int main(void) {

	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
	BOARD_InitDebugConsole();
#endif

	init_ticktime();

	PRINTF("Running validity tests...\r\n");
	run_tests();
	PRINTF("All tests passed!\r\n");

	//Time test section 1 for reporting and comparing time.
	PRINTF("Running timing test...Report this time.\r\n");
	time_pbkdf1(true);
	PRINTF("Done with timing test...\r\n");

	//Time test section 2 for profiling with static profiling.
	PRINTF("Running call count test with static profiling....\r\n");
	static_profile_on();
	time_pbkdf1(false);
	static_profile_off();
	print_static_profiler_summary();
	PRINTF("Done with call count test with static profiling....\r\n");

	//Time test section 3 for profiling with PC.
	// Note: The PC profiler requires all profiled functions to already
	// have been run in order to capture their end locations in memory.
	// This is OK in this case because we have already run
	// tests that fully exercised the relevant functions.
	PRINTF("Running call count test with PC profiling....\r\n");
	pc_profile_on();
	time_pbkdf1(false);
	pc_profile_off();
	print_pc_profiler_summary();
	PRINTF("Done with call count test with PC profiling....\r\n");

	return 0;
}

