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
 * @file    test_llfifo.c
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
#include <time.h>
#include "llfifo.h"

/* ---------- */
/* Test cases */
/* ---------- */
int test_llfifo_lifecycle()
{
    puts("Testing creation of llfifo queue...");
    llfifo_t *fifo = llfifo_create(3, 5);
    int len = llfifo_length(fifo);
    assert(len == 0);
    int capacity = llfifo_capacity(fifo);
    assert(capacity == 3);
    int max_capacity = llfifo_max_capacity(fifo);
    assert(max_capacity == 5);
    llfifo_destroy(fifo);
    puts("Passed");
    return 0;
}

int test_given_assignment_test_case()
{
    puts("Testing standard assignment test case...");
    llfifo_t *my_FIFO = llfifo_create(5, 100);
    assert(llfifo_length(my_FIFO) == 0);
    assert(llfifo_capacity(my_FIFO) == 5);
    assert(llfifo_max_capacity(my_FIFO) == 100);
    llfifo_enqueue(my_FIFO, "Sleepy");   // does not result in a malloc (len = 1, cap = 5)
    llfifo_enqueue(my_FIFO, "Grumpy");   // does not result in a malloc (len = 2, cap = 5)
    llfifo_enqueue(my_FIFO, "Sneezy");   // does not result in a malloc (len = 3, cap = 5)
    llfifo_enqueue(my_FIFO, "Happy");    // does not result in a malloc (len = 4, cap = 5)
    assert(llfifo_length(my_FIFO) == 4);
    llfifo_enqueue(my_FIFO, "Bashful");  // does not result in a malloc (len = 5, cap = 5)
    assert(llfifo_length(my_FIFO) == 5);
    puts("Dequeueing Sleepy");
    char *popped = llfifo_dequeue(my_FIFO);  // removes "Sleepy"; len = 4, cap = 5
    puts("Done");
    assert(strcmp(popped, "Sleepy") == 0);
    puts("A");
    assert(llfifo_capacity(my_FIFO) == 5);
    puts("B");
    assert(llfifo_length(my_FIFO) == 4);
    puts("Enqueueing Dopey");
    llfifo_enqueue(my_FIFO, "Dopey");    // does not result in a malloc (len = 5, cap = 5)
    llfifo_enqueue(my_FIFO, "Doc");      // DOES result in a malloc (len = 6, cap = 6)
    assert(llfifo_capacity(my_FIFO) == 6);
    // Pop Grumpy
    popped = llfifo_dequeue(my_FIFO);
    assert(llfifo_capacity(my_FIFO) == 6);
    assert(llfifo_length(my_FIFO) == 5);
    assert(strcmp(popped, "Grumpy") == 0);
    // Pop Sneezy
    popped = llfifo_dequeue(my_FIFO);
    assert(llfifo_capacity(my_FIFO) == 6);
    assert(llfifo_length(my_FIFO) == 4);
    assert(strcmp(popped, "Sneezy") == 0);
    // Destroy queue
    llfifo_destroy(my_FIFO);
    my_FIFO = NULL;
    puts("Passed");
    return 0;
}

int test_capacity_management()
{
    puts("Testing capacity management...");
    llfifo_t *my_FIFO = llfifo_create(1, 3);
    assert(llfifo_length(my_FIFO) == 0);
    assert(llfifo_capacity(my_FIFO) == 1);
    assert(llfifo_max_capacity(my_FIFO) == 3);

    /* Enqueuing operations */
    int valneg1 = -1;
    llfifo_enqueue(my_FIFO, &valneg1);  // This should use preallocated capacity
    assert(llfifo_length(my_FIFO) == 1);
    assert(llfifo_capacity(my_FIFO) == 1);

    int val0 = 0;
    llfifo_enqueue(my_FIFO, &val0);  // This creates a new node
    assert(llfifo_length(my_FIFO) == 2);
    assert(llfifo_capacity(my_FIFO) == 2);

    int val1 = 1;
    llfifo_enqueue(my_FIFO, &val1);  // This creates a new node
    assert(llfifo_length(my_FIFO) == 3);
    assert(llfifo_capacity(my_FIFO) == 3);

    int val2 = 2;
    llfifo_enqueue(my_FIFO, &val2);  // This overflows max capacity and overwrites value -1
    assert(llfifo_length(my_FIFO) == 3);
    assert(llfifo_capacity(my_FIFO) == 3);

    int val3 = 3;
    llfifo_enqueue(my_FIFO, &val3);  // This overflows max capacity and overwrites value 0
    assert(llfifo_length(my_FIFO) == 3);
    assert(llfifo_capacity(my_FIFO) == 3);
 
    /* Dequeueing operations */
    /* Since the values -1 and 0 got overwritten by 2 and 3 respectively, we expect the first value to be 1 */
    assert(llfifo_length(my_FIFO) == 3);
    assert(*(int *)llfifo_dequeue(my_FIFO) == 1);
    assert(llfifo_capacity(my_FIFO) == 3);
    assert(llfifo_max_capacity(my_FIFO) == 3);

    /* Now 2 */
    assert(llfifo_length(my_FIFO) == 2);
    assert(*(int *)llfifo_dequeue(my_FIFO) == 2);
    assert(llfifo_capacity(my_FIFO) == 3);
    assert(llfifo_max_capacity(my_FIFO) == 3);

    /* Now 3 */
    assert(llfifo_length(my_FIFO) == 1);
    assert(*(int *)llfifo_dequeue(my_FIFO) == 3);
    assert(llfifo_capacity(my_FIFO) == 3);
    assert(llfifo_max_capacity(my_FIFO) == 3);

    /* We should now have an empty list with max capacity */
    assert(llfifo_length(my_FIFO) == 0);
    assert(llfifo_capacity(my_FIFO) == 3);

    puts("Passed");
    return 0;
}

int test_large_queue()
{
    puts("Testing large queue...");
    int NUM_LOOPS = 1000000;
    int NUM_CAPACITY = NUM_LOOPS - 1000;
    printf("capacity=%d, max_capacity=%d\n", NUM_CAPACITY, NUM_LOOPS);
    clock_t begin, end;
    begin = clock();
    llfifo_t *my_FIFO = llfifo_create(NUM_CAPACITY, NUM_LOOPS);
    end = clock();
    printf("Time taken to preallocate capacity:%lf\n",(double)(end-begin)/CLOCKS_PER_SEC);

    begin = clock();
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        assert(llfifo_length(my_FIFO) == i);
        assert(llfifo_enqueue(my_FIFO, "ABC") > 0);
    }
    end = clock();
    printf("Time taken to fill queue:%lf\n",(double)(end-begin)/CLOCKS_PER_SEC);

    assert(llfifo_length(my_FIFO) == NUM_LOOPS);

    begin = clock();
    for (int i = NUM_LOOPS; i > 0; i--)
    {
        assert(llfifo_length(my_FIFO) == i);
        assert(strcmp((char *)llfifo_dequeue(my_FIFO), "ABC") == 0);
    }
    end = clock();
    printf("Time taken to dequeue all values:%lf\n",(double)(end-begin)/CLOCKS_PER_SEC);

    assert(llfifo_length(my_FIFO) == 0);
    assert(llfifo_capacity(my_FIFO) == NUM_LOOPS);
    assert(llfifo_max_capacity(my_FIFO) == NUM_LOOPS);

    begin = clock();
    puts("destroying fifo and list");
    llfifo_destroy(my_FIFO);
    end = clock();
    printf("Time taken to destroy:%lf",(double)(end-begin)/CLOCKS_PER_SEC);
    puts("Passed");
    return 0;
}

int test_edge_cases()
{
    puts("Testing edge cases...");
    llfifo_t *null_fifo = llfifo_create(0, 0);
    assert(!null_fifo);

    llfifo_t *zero_preallocated = llfifo_create(0, 5);
    assert(llfifo_length(zero_preallocated) == 0);
    llfifo_enqueue(zero_preallocated, "First");
    assert(llfifo_length(zero_preallocated) == 1);
    assert(strcmp(llfifo_dequeue(zero_preallocated), "First") == 0);
    assert(llfifo_length(zero_preallocated) == 0);
    assert(llfifo_capacity(zero_preallocated) == 1);
    assert(!llfifo_dequeue(zero_preallocated));  // dequeue an empty queue gives you NULL
    assert(llfifo_length(zero_preallocated) == 0);
    // Enqueue NULL should produce -1
    assert(llfifo_enqueue(zero_preallocated, NULL) == -1);
    llfifo_destroy(zero_preallocated);
    
    // Negative capacity should fail
    assert(!llfifo_create(-1, 5));

    // Max capacity less than capacity should fail
    assert(!llfifo_create(5, 3));

    // Enqueue on NULL pointer should return -1
    assert(llfifo_enqueue(NULL, "Fail") == -1);

    // Dequeue on NULL pointer should return NULL
    assert(!llfifo_dequeue(NULL));

    // Getters should return NULL if fifo is NULL
    assert(llfifo_length(NULL) == -1);
    assert(llfifo_capacity(NULL) == -1);
    assert(llfifo_max_capacity(NULL) == -1);

    // Create and immediately destroy a queue with zero nodes
    llfifo_t *null_queue = llfifo_create(0, 5);
    llfifo_destroy(null_queue);

    puts("Passed");
    return 0;
}

/* ------------------ */
/* Main test function */
/* ------------------ */
int test_llfifo()
{
    puts("Testing the llfifo module...");
    assert(test_llfifo_lifecycle() == 0);
    assert(test_given_assignment_test_case() == 0);
    assert(test_capacity_management() == 0);
    assert(test_large_queue() == 0);
    assert(test_edge_cases() == 0);
    puts("All llfifo module tests passed");
    return 0;
}
