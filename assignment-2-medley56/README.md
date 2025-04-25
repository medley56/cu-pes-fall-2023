# PES Assignment 2: FIFOs

This project is built with `make`. Simply run `make` in the repo root to build with defaults parameters. The resulting executable will be called `assgn2`.

Error codes are `0xFFFFFFFFU` for all functions that return numeric values and `NULL` for functions that return pointers.

# Main Function

The main function of this repo runs test cases against the `llfifo` and `cbfifo` module. The test cases are comprehensive and test basic functionality as well as covering edge cases. See `test_cbfifo.c` and `test_llfifo.c`.

# `llfifo` Module

The `llfifo` module contains an API for a FIFO queue backed by a linked list. 

## Basic Usage
```c
llfifo_t *my_FIFO = llfifo_create(5, 100);  // Creates a queue with 5 preallocated nodes and max 100 nodes
llfifo_enqueue(my_FIFO, "Sleepy");
llfifo_enqueue(my_FIFO, "Grumpy");
char *popped = llfifo_dequeue(my_FIFO);  // Dequeue ("pop") the oldest item
puts(popped);  // Sleepy
llfifo_destroy(my_FIFO);  // Frees memory
my_FIFO = NULL;
```

Note: the `llfifo` queue implementation is thread safe! Each instance of `llfifo_t` is dynamically allocated and cleaned up on destruction or program exit.

# `cbfifo` Module

The `cbfifo` module contains an API for a FIFO queue backed by a (global) circular buffer.

It has a hard-coded maximum capacity of 127 bytes and bytes may be enqueued and dequeued one at a time according to the `nbyte` parameter passed to `cbfifo_enqueue` and `cbfifo_dequeue`. 

## Basic Usage
```c
cbfifo_reset();  // Ensures no data in buffer
cbfifo_enqueue(
        "CBB0A0aB#2??2bcAbcbA1c!c0a!#0A!A!B0?bC!b2c!1b10b??11a1!!!c0!aba?!?c"
        "?1ab?C2#a1A#a2?ACAC#BbaBc212?!c1B!#!cBAC!!aCCC22aC0CBCa!!A?b",  // String is length 127
        127  // Enqueues maximum number of bytes
        );
char buf[4] = { '\0' };
cbfifo_dequeue(buf, 3);
puts(buf);  // CBB
cbfifo_reset();  // Clears buffer 
```

Note: The `cbfifo` queue implementation is not suitable for multiprocessing/multithreading environments. `cbfifo` is NOT thread safe.


