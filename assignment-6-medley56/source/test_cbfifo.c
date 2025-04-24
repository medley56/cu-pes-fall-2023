/**
 * @file test_cbfifo.c
 * @brief Tests for cbfifo queue implementation
 *
 * @author Gavin Medley
 * @date 2023-09-20
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cbfifo.h"

/* ---------- */
/* Test cases */
/* ---------- */
int test_basic_usage()
{
	// puts("Testing basic usage of the cbfifo...");
	cbfifo_reset(Q_RX);  // Ensures no data in buffer
	cbfifo_enqueue(
	Q_RX, "CBB0A0aB#2??2bcAbcbA1c!c0a!#0A!A!B0?bC!b2c!1b10b??11a1!!!c0!aba?!?c"
			"?1ab?C2#a1A#a2?ACAC#BbaBc212?!c1B!#!cBAC!!aCCC22aC0CBCa!!A?b", // String is length 127
			127  // Enqueues maximum number of bytes
			);
	char buf[4] = { '\0' };
	cbfifo_dequeue(Q_RX, buf, 3);
	// puts(buf);  // CBB
	cbfifo_reset(Q_RX);
	// puts("Passed");
	return 0;
}

int test_complex_usage()
{
	// puts("Testing complex usage of the cbfifo...");
	assert(cbfifo_capacity(Q_RX) == 127);
	// puts("Capacity passed");
	assert(cbfifo_length(Q_RX) == 0);
	// puts("Length passed");
	cbfifo_reset(Q_RX);  // Check that reset doesn't have side effects
	// puts("Reset passed");
	assert(cbfifo_capacity(Q_RX) == 127);
	assert(cbfifo_length(Q_RX) == 0);

	// Check that we can enqueue bytes and the length is affected as expected
	size_t copied = cbfifo_enqueue(Q_RX, "Sleepy", 6);
	// printf("Copied %lu bytes\n", copied);
	assert(copied == 6);
	// puts("Enqueue passed");
	assert(cbfifo_length(Q_RX) == 6);

	// Check that reset does actually reset the queue
	cbfifo_reset(Q_RX);
	assert(cbfifo_capacity(Q_RX) == 127);
	assert(cbfifo_length(Q_RX) == 0);

	// Test enqueuing more than the maximum number of bytes
	size_t n;
	n = cbfifo_enqueue(
	Q_RX, "CBB0A0aB#2??2bcAbcbA1c!c0a!#0A!A!B0?bC!b2c!1b10b??11a1!!!c0!aba?!?c"
			"?1ab?C2#a1A#a2?ACAC#BbaBc212?!c1B!#!cBAC!!aCCC22aC0CBCa!!A?b2", // The 2 should be excluded
			128);
	assert(n == 127);  // 127 bytes enqueued

	// Test dequeuing the entire queue with a nbyte number more than the buffer size
	char buf[127];
	n = cbfifo_dequeue(Q_RX, buf, 200);
	assert(n == 127);
	assert(cbfifo_length(Q_RX) == 0);
	assert(
			strcmp(buf,
					"CBB0A0aB#2??2bcAbcbA1c!c0a!#0A!A!B0?bC!b2c!1b10b??11a1!!!c0!aba?!?c"
							"?1ab?C2#a1A#a2?ACAC#BbaBc212?!c1B!#!cBAC!!aCCC22aC0CBCa!!A?b" // Note that the 128th char (2) is trimmed off
					) == 0);
	cbfifo_reset(Q_RX);  // Must reset the queue before running any more tests
	// puts("Passed");
	return 0;
}

int test_cbfifo_edge_cases()
{
	// puts("Testing cbfifo edge cases...");
	assert(cbfifo_enqueue(Q_RX, NULL, 1) == 0);
	assert(cbfifo_enqueue(Q_RX, "WONT ENQUEUE", 0) == 0);

	// Show that we don't care what bytes we are enqueueing (e.g. a null terminated string doesn't cause issues)
	size_t n = cbfifo_enqueue(Q_RX, "WILL ENQUEUE", 1000);
	assert(n == 127);
	cbfifo_reset(Q_RX); // Must reset queue before we run any other tests
	// puts("Passed");
	return 0;
}

int test_cbfifo_multiple_queues()
{
	// puts("Testing cbfifo multiple queues...");
	cbfifo_enqueue(Q_RX, "Sleepy", 6);
	assert(cbfifo_length(Q_RX) == 6);
	assert(cbfifo_length(Q_TX) == 0);
	cbfifo_enqueue(Q_TX, "0123456789", 10);
	assert(cbfifo_length(Q_TX) == 10);
	assert(cbfifo_length(Q_RX) == 6);
	cbfifo_reset(Q_TX);
	assert(cbfifo_length(Q_RX) == 6);
	assert(cbfifo_length(Q_TX) == 0);
	cbfifo_reset(Q_RX);
	cbfifo_reset(Q_TX);
	// puts("Passed");
	return 0;
}

/* ------------------ */
/* Main test function */
/* ------------------ */
int test_cbfifo()
{
	// puts("Testing the cbfifo module...");
	cbfifo_init();  // Initialize the cbfifo module
	cbfifo_reset(Q_RX);
	cbfifo_reset(Q_TX);
	assert(test_basic_usage() == 0);
	assert(test_complex_usage() == 0);
	assert(test_cbfifo_edge_cases() == 0);
	assert(test_cbfifo_multiple_queues() == 0);
	// puts("All cbfifo module tests passed");
	return 0;
}
