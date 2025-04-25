/*
 * profiler.c
 *
 *  Created on: Aug 2, 2023
 *      Author: lpandit
 */

#include "MKL25Z4.h"
#include "fsl_debug_console.h"
#include "pc_profiler.h"
#include "isha.h"

bool pc_profiling_on = false;

// Count variables for counting systick events that occur in each function of interest
uint32_t ISHAProcessMessageBlockCountPC, ISHAPadMessageCountPC,
		ISHAResetCountPC, ISHAInputCountPC, ISHAResultCountPC;
uint32_t UnknownCountPC;

// Struct type that stores the start and end address for a function
typedef struct {
	uint32_t start;
	uint32_t end;
} AddressRange;

// Struct to store the address range for each function of interest
AddressRange ISHAProcessMessageBlockAddr, ISHAPadMessageAddr, ISHAResetAddr,
		ISHAInputAddr, ISHAResultAddr;

/*
 * Store the address ranges for each of the functions of interest so
 * that we don't have to call GetFunctionAddress on every systick IRQ
 */
void store_pc_ranges(void) {
	PRINTF("Storing address ranges for each function of interest...\r\n");
	uint32_t start, end;
	GetFunctionAddress("ISHAProcessMessageBlock", &start, &end);
	ISHAProcessMessageBlockAddr = (AddressRange ) { start, end };
	PRINTF("ISHAProcessMessageBlock start=%u, end=%u\r\n",
			ISHAProcessMessageBlockAddr.start, ISHAProcessMessageBlockAddr.end);
	GetFunctionAddress("ISHAPadMessage", &start, &end);
	ISHAPadMessageAddr = (AddressRange ) { start, end };
	PRINTF("ISHAPadMessage start=%u, end=%u\r\n", ISHAPadMessageAddr.start,
			ISHAPadMessageAddr.end);
	GetFunctionAddress("ISHAReset", &start, &end);
	ISHAResetAddr = (AddressRange ) { start, end };
	PRINTF("ISHAReset start=%u, end=%u\r\n", ISHAResetAddr.start,
			ISHAResetAddr.end);
	GetFunctionAddress("ISHAInput", &start, &end);
	ISHAInputAddr = (AddressRange ) { start, end };
	PRINTF("ISHAInput start=%u, end=%u\r\n", ISHAInputAddr.start,
			ISHAInputAddr.end);
	GetFunctionAddress("ISHAResult", &start, &end);
	ISHAResultAddr = (AddressRange ) { start, end };
	PRINTF("ISHAResult start=%u, end=%u\r\n", ISHAResultAddr.start,
			ISHAResultAddr.end);
}

/*
 * Check the given PC value and determine which function it likely
 * belongs to.
 *
 * Parameters
 *   pc - Pointer to a uint32_t that stores a PC value.
 */
void pc_profile_check(uint32_t *pc) {
	// Compare pc value to the range of addresses for each function in the ISHA module
	//PRINTF("%u\r\n", *pc);
	if (*pc >= ISHAProcessMessageBlockAddr.start
			&& *pc < ISHAProcessMessageBlockAddr.end) {
		ISHAProcessMessageBlockCountPC++;
		return;
	}

	if (*pc >= ISHAPadMessageAddr.start && *pc < ISHAPadMessageAddr.end) {
		ISHAPadMessageCountPC++;
		return;
	}

	if (*pc >= ISHAResetAddr.start && *pc < ISHAResetAddr.end) {
		ISHAResetCountPC++;
		return;
	}

	if (*pc >= ISHAInputAddr.start && *pc < ISHAInputAddr.end) {
		ISHAInputCountPC++;
		return;
	}

	if (*pc >= ISHAResultAddr.start && *pc < ISHAResultAddr.end) {
		ISHAResultCountPC++;
		return;
	}

	// If our PC doesn't match any of the relevant function address ranges,
	// increment the unknown state counter
	UnknownCountPC++;
}

/*
 * Turn the profiler on for call counting.
 */
void pc_profile_on(void) {
	// Store the start and end locations for each function once
	store_pc_ranges();
	pc_profiling_on = true;
}

/*
 * Turn the profiler off for call counting.
 */
void pc_profile_off(void) {
	pc_profiling_on = false;
}

/*
 * Print a summary of profiler calls for following functions.
 *
 */
void print_pc_profiler_summary(void) {
	PRINTF("PC profiling results (Call count indicates the number of PC samples taken in the function):\r\n");
	PRINTF("Function: ISHAProcessMessageBlock    Call count: %u\r\n",
			ISHAProcessMessageBlockCountPC);
	PRINTF("Function: ISHAPadMessage             Call count: %u\r\n", ISHAPadMessageCountPC);
	PRINTF("Function: ISHAReset                  Call count: %u\r\n", ISHAResetCountPC);
	PRINTF("Function: ISHAInput                  Call count: %u\r\n", ISHAInputCountPC);
	PRINTF("Function: ISHAResult                 Call count: %u\r\n", ISHAResultCountPC);
	PRINTF("Function: Unknown                    Call count: %u\r\n", UnknownCountPC);
	PRINTF("End of PC profiling results\r\n");
}

