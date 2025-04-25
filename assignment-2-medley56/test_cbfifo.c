/*******************************************************************************
 * Copyright (C) 2023 by Gavin Medley
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are
 * permitted to modify this and use it to learn about the field of embedded
 * software. Gavin Medley and the University of Colorado are not liable for
 * any misuse of this material.
 * ****************************************************************************/

/**
 * @file    test_cbfifo.c
 * @brief
 *
 *
 * @author  Gavin Medley
 * @date    2023-09-20
 *
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
	uint8_t rt = 0;
    puts("Testing basic usage of the cbfifo...");
    cbfifo_reset(rt);  // Ensures no data in buffer
    cbfifo_enqueue(
    		rt,
            "CBB0A0aB#2??2bcAbcbA1c!c0a!#0A!A!B0?bC!b2c!1b10b??11a1!!!c0!aba?!?c"
            "?1ab?C2#a1A#a2?ACAC#BbaBc212?!c1B!#!cBAC!!aCCC22aC0CBCa!!A?b",  // String is length 127
            127  // Enqueues maximum number of bytes
            );
    char buf[4] = { '\0' };
    cbfifo_dequeue(rt, buf, 3);
    puts(buf);  // CBB
    cbfifo_reset(rt);
    puts("Passed");
    return 0;
}

int test_complex_usage()
{
	uint8_t rt = 0;
    puts("Testing complex usage of the cbfifo...");
    assert(cbfifo_capacity(rt) == 127);
    puts("Capacity passed");
    assert(cbfifo_length(rt) == 0);
    puts("Length passed");
    cbfifo_reset(rt);  // Check that reset doesn't have side effects
    puts("Reset passed");
    assert(cbfifo_capacity(rt) == 127);
    assert(cbfifo_length(rt) == 0);

    // Check that we can enqueue bytes and the length is affected as expected
    size_t copied = cbfifo_enqueue(rt, "Sleepy", 6);
    printf("Copied %lu bytes\n", copied);
    puts("Enqueue passed");
    assert(cbfifo_length(rt) == 6);

    // Check that reset does actually reset the queue
    cbfifo_reset(rt);
    assert(cbfifo_capacity(rt) == 127);
    assert(cbfifo_length(rt) == 0);

    // Test enqueuing more than the maximum number of bytes
    size_t n;
    n = cbfifo_enqueue(
    		rt,
			"CBB0A0aB#2??2bcAbcbA1c!c0a!#0A!A!B0?bC!b2c!1b10b??11a1!!!c0!aba?!?c"
			"?1ab?C2#a1A#a2?ACAC#BbaBc212?!c1B!#!cBAC!!aCCC22aC0CBCa!!A?b2",  // The 2 should be excluded
			128
        	);
    assert(n == 127);  // 127 bytes enqueued

    // Test dequeuing the entire queue with a nbyte number more than the buffer size
    char buf[127];
    n = cbfifo_dequeue(rt, buf, 200);
    assert(n == 127);
    assert(cbfifo_length(rt) == 0);
    assert(strcmp(
        buf, 
        "CBB0A0aB#2??2bcAbcbA1c!c0a!#0A!A!B0?bC!b2c!1b10b??11a1!!!c0!aba?!?c"
        "?1ab?C2#a1A#a2?ACAC#BbaBc212?!c1B!#!cBAC!!aCCC22aC0CBCa!!A?b"  // Note that the 128th char (2) is trimmed off
        ) == 0);
    cbfifo_reset(rt);  // Must reset the queue before running any more tests
    puts("Passed");
    return 0;
}

int test_cbfifo_edge_cases()
{
	uint8_t rt = 0;
    puts("Testing cbfifo edge cases...");
    assert(cbfifo_enqueue(rt, NULL, 1) == 0);
    assert(cbfifo_enqueue(rt, "WONT ENQUEUE", 0) == 0);

    // Show that we don't care what bytes we are enqueueing (e.g. a null terminated string doesn't cause issues)
    size_t n = cbfifo_enqueue(rt, "WILL ENQUEUE", 1000);
    assert(n == 127);
    cbfifo_reset(rt); // Must reset queue before we run any other tests
    puts("Passed");
    return 0;
}

int test_cbfifo_multiple_queues()
{
    puts("Testing cbfifo multiple queues...");
    uint8_t rt0 = 0;
    uint8_t rt1 = 1;
    size_t n = cbfifo_enqueue(rt0, "Sleepy", 6);
    assert(cbfifo_length(rt0) == 6);
    assert(cbfifo_length(rt1) == 0);
    cbfifo_enqueue(rt1, "0123456789", 10);
    assert(cbfifo_length(rt1) == 10);
    assert(cbfifo_length(rt0) == 6);
    cbfifo_reset(rt1);
    assert(cbfifo_length(rt0) == 6);
    assert(cbfifo_length(rt1) == 0);
    cbfifo_reset(rt0);
    cbfifo_reset(rt1);
    puts("Passed");
    return 0;
}

/* ------------------ */
/* Main test function */
/* ------------------ */
int test_cbfifo()
{
    puts("Testing the cbfifo module...");
    cbfifo_init();  // Initialize the cbfifo module
    assert(test_basic_usage() == 0);
    assert(test_complex_usage() == 0);
    assert(test_cbfifo_edge_cases() == 0);
    assert(test_cbfifo_multiple_queues() == 0);
    puts("All cbfifo module tests passed");
    return 0;
}
