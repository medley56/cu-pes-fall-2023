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
 * @file    cbfifo.c
 * @brief
 *
 *
 * @author  Gavin Medley
 * @date    2023-09-20
 *
 */
#include <string.h>
#include "cbfifo.h"

#define ERROR 0xFFFFFFFFU  // Generic ERROR code (-1 unsigned)
#define BUFFER_SIZE 127U  // Length of circular buffer, unsigned, in bytes

// Global variables for RECEIVE circular buffer
typedef struct circular_buffer_s {
	uint8_t BUFFER[BUFFER_SIZE];  // zeros
	uint8_t *READ_PTR;  // pointer to BUFFER
	uint8_t *WRITE_PTR;  // pointer to BUFFER
	uint8_t BUFFER_FULL;  // false
	uint8_t CAPACITY;  // always set to BUFFER_SIZE
} CircularBuffer_t;

/* Declare the transmit and receive queues.
READ_PTR and WRITE_PTR are initialized to NULL until cbfifo_init is called. */
CircularBuffer_t tx_queue = {
    .BUFFER = {0},
    .READ_PTR = NULL,
    .WRITE_PTR = NULL,
    .BUFFER_FULL = 0,
    .CAPACITY = BUFFER_SIZE
};
CircularBuffer_t rx_queue = {
    .BUFFER = {0},
    .READ_PTR = NULL,
    .WRITE_PTR = NULL,
    .BUFFER_FULL = 0,
    .CAPACITY = BUFFER_SIZE
};


/*
 * Get the queue pointer determined by the rt value
 * 
 * Parameters
 *   rt  Receive (0) or transmit (1) queue to retrieve
 */
CircularBuffer_t *cbfifo_get_queue(uint8_t rt)
{
    if (rt == 0)  // Receive queue
	{
		return &rx_queue;
	} else if (rt == 1)  // Transmit queue
	{
		return &tx_queue;
	} else  // Unknown queue
	{
		return NULL;
	}
}


void cbfifo_init()
{
	/* Initialize the transmit queue pointers */
	tx_queue.READ_PTR = &tx_queue.BUFFER[0];
	tx_queue.WRITE_PTR = &tx_queue.BUFFER[0];

	/* Initialize the receive queue pointers */
	rx_queue.READ_PTR = &rx_queue.BUFFER[0];
	rx_queue.WRITE_PTR = &rx_queue.BUFFER[0];
}


size_t cbfifo_enqueue(uint8_t rt, void *buf, size_t nbyte)
{
	/* Choose a queue based on rt (receive/transmit) value */
	CircularBuffer_t *qptr = cbfifo_get_queue(rt);

    /* Assess how much unused capacity is available in the buffer */
    size_t nunused = cbfifo_capacity(rt) - cbfifo_length(rt);
    /* Check for edge case values to return early */
    if (nunused < 1 || nbyte < 1 || !buf)
    {
        return 0;
    }

    /* Determine exactly how many bytes to copy into the fifo buffer (minimum of nbyte and nunused) */
    size_t ncopy;
    if (nunused >= nbyte)
    {
        ncopy = nbyte;
    } else
    {
        ncopy = nunused;
    }

    if (ncopy > &qptr->BUFFER[0] + qptr->CAPACITY - qptr->WRITE_PTR)
    {
        /* Indicates that we are enqueueing more bytes than are available between WRITE_PTR and the end of the BUFFER.
        This requires two memcpy calls */

        /* First enqueue data from WRITE_PTR to end of BUFFER */
        size_t ntailing = qptr->CAPACITY - (qptr->WRITE_PTR - &qptr->BUFFER[0]);  // Number of bytes between WRITE_PTR and end of BUFFER
        if (!memcpy(qptr->WRITE_PTR, buf, ntailing))
        {
            return ERROR;
        }
        buf += ntailing;  // Increment incoming buffer by the number of bytes we just enqueued

        /* Then enqueue data from &BUFFER[0] possibly up to READ_PTR but possibly fewer */
        if (!memcpy(&qptr->BUFFER[0], buf, ncopy - ntailing))
        {
            return ERROR;
        }

        /* Move WRITE_PTR to end of recently enqueued bytes (possibly matching READ_PTR) */
        qptr->WRITE_PTR = &qptr->BUFFER[0] + (ncopy - ntailing);
    } else
    {
        /* Indicates that all bytes to be enqueued can fit between WRITE_PTR and the end of BUFFER.
        We can copy all the requisite bytes in one memcpy call */
        if (!memcpy(qptr->WRITE_PTR, buf, ncopy))
        {
            return ERROR;
        }

        /* Increment WRITE_PTR forward by number of bytes copied */
        qptr->WRITE_PTR += ncopy;
    }

    /* Check if buffer is full and set BUFFER_FULL to 1 if so */
    if (qptr->READ_PTR == qptr->WRITE_PTR)
    {
    	qptr->BUFFER_FULL = 1;
    }

    /* Return the number of bytes actually copied */
    return ncopy;
}

size_t cbfifo_dequeue(uint8_t rt, void *buf, size_t nbyte)
{
    /* Assess how much data is available in the buffer */
    size_t navailable = cbfifo_length(rt);

    /* Check for edge case values */
    if (navailable < 1 || nbyte < 1 || !buf)
    {
        return 0;
    }

    /* Determine exactly how many bytes to copy out of the fifo buffer (minimum of nbyte and navailable) */
    size_t ncopy;  // Number of bytes to copy
    if (navailable >= nbyte)
    {
        ncopy = nbyte;
    } else
    {
        ncopy = navailable;
    }

    /* Choose a queue based on rt (receive/transmit) value */
	CircularBuffer_t *qptr = cbfifo_get_queue(rt);

    if (ncopy > &qptr->BUFFER[0] + qptr->CAPACITY - qptr->READ_PTR)
    {
        /* Indicates that we are copying more bytes than are available between READ_PTR and the end of the BUFFER.
        This requires two memcpy calls */

        /* First copy from READ_PTR to end of BUFFER */
        size_t ntailing = qptr->CAPACITY - (qptr->READ_PTR - &qptr->BUFFER[0]);
        if (!memcpy(buf, qptr->READ_PTR, ntailing))
        {
            return ERROR;
        }
        buf += ntailing;  // Increment output buffer by number of bytes we just dequeued

        /* Then copy from &BUFFER[0] possibly up to WRITE_PTR but possibly fewer */
        if (!memcpy(buf, &qptr->BUFFER[0], ncopy - ntailing))
        {
            return ERROR;
        }

        /* Move READ_PTR to in front of WRITE_PTR (or possibly to same location) */
        qptr->READ_PTR = &qptr->BUFFER[0] + (ncopy - ntailing);
    } else
    {
        /* Indicates that all bytes to be copied exist between READ_PTR and the end of BUFFER.
        We can copy all the requisite bytes in one memcpy call */
        if (!memcpy(buf, qptr->READ_PTR, ncopy))
        {
            return ERROR;
        }

        /* Increment READ_PTR forward by number of bytes copied */
        qptr->READ_PTR += ncopy;
    }

    /* Check if buffer is empty and set BUFFER_FULL to 0 if so */
    if (qptr->READ_PTR == qptr->WRITE_PTR)
    {
    	qptr->BUFFER_FULL = 0;
    }

    /* Return the number of bytes actually copied */
    return ncopy;
}

size_t cbfifo_length(uint8_t rt)
{
	/* Choose a queue based on rt (receive/transmit) value */
	CircularBuffer_t *qptr = cbfifo_get_queue(rt);

    /* If READ_PTR is NULL, things are broken somehow */
    if (!qptr->READ_PTR || !qptr->WRITE_PTR)
    {
        return ERROR;
    }

    /* Get wraparound length */
    size_t length;
    if (qptr->READ_PTR > qptr->WRITE_PTR)
    {
        /*
              |write
        |*****|--------|****|
                       |read
        */
        length = qptr->CAPACITY - (qptr->READ_PTR - qptr->WRITE_PTR);
    } else if ( qptr->READ_PTR < qptr->WRITE_PTR )
    {
        /*
              |read
        |-----|********|----|
                       |write
        */
       length = qptr->WRITE_PTR - qptr->READ_PTR;
    } else
    {
        /* Case when READ_PTR == WRITE_PTR */
        if (qptr->BUFFER_FULL)
        {
            length = qptr->CAPACITY;
        } else
        {
            length = 0;
        }
    }
    return length;
}

size_t cbfifo_capacity(uint8_t rt)
{
	/* Choose a queue based on rt (receive/transmit) value */
	CircularBuffer_t *qptr = cbfifo_get_queue(rt);
	return qptr->CAPACITY;
}

void cbfifo_reset(uint8_t rt)
{
	/* Choose a queue based on rt (receive/transmit) value */
	CircularBuffer_t *qptr = cbfifo_get_queue(rt);

    /* Reset the read and write pointers to the beginning of the BUFFER */
	qptr->READ_PTR = &qptr->BUFFER[0];
    qptr->WRITE_PTR = &qptr->BUFFER[0];

    return;
}
