/*
 * cbfifo.h - a fixed-size FIFO implemented for two circular buffers, intended
 * for queueing data for one read queue and one write queue.
 *
 * Author: Howdy Pierce, Lalit Pandit, Gavin Medley
 *
 */

#ifndef _CBFIFO_H_
#define _CBFIFO_H_

#include <stdlib.h>  // for size_t
#include <stdint.h>

/*
 * Initialize the RECEIVE and TRANSMIT queues
 */
void cbfifo_init();

/*
 * Enqueues data onto the specified FIFO, up to the limit of the available FIFO
 * capacity.
 *
 * Parameters:
 *   rt       Receive (0) or transmit (1) queue
 *   buf      Pointer to the data
 *   nbyte    Max number of bytes to enqueue
 *
 * Returns:
 *   The number of bytes actually enqueued, which could be 0. In case
 *   of an error, returns (size_t) -1. For example, if the capacity is 30,
 *   and current lenght of FIFO is 0. A call to enqueue 35 bytes, will
 *   only enqueue 30 to the capacity and return 30 from this call. Application
 *   needs to retry enqueuing the remaining 5 bytes subsequently.
 */
size_t cbfifo_enqueue(uint8_t rt, void *buf, size_t nbyte);


/*
 * Attempts to remove ("dequeue") up to nbyte bytes of data from the
 * FIFO. Removed data will be copied into the buffer pointed to by buf.
 *
 * Parameters:
 *   rt  Receive (0) or trasmit (1) queue
 *   buf      Destination for the dequeued data
 *   nbyte    Bytes of data requested
 *
 * Returns:
 *   The number of bytes actually copied, which will be between 0 and
 *   nbyte(inclusive of both).
 *
 * To further explain the behavior: If the FIFO's current length is 24
 * bytes, and the caller requests 30 bytes, cbfifo_dequeue should
 * return the 24 bytes it has, and the new FIFO length will be 0. If
 * the FIFO is empty (current length is 0 bytes), a request to dequeue
 * any number of bytes will result in a return of 0 from
 * cbfifo_dequeue.
 */
size_t cbfifo_dequeue(uint8_t rt, void *buf, size_t nbyte);


/*
 * Returns the number of bytes currently on the FIFO.
 *
 * Parameters:
 *   rt  Receive (0) or trasmit (1) queue
 *
 * Returns:
 *   Number of bytes currently available to be dequeued from the FIFO
 */
size_t cbfifo_length(uint8_t rt);


/*
 * Returns the FIFO's capacity
 *
 * Parameters:
 *   rt  Receive (0) or trasmit (1) queue
 *
 * Returns:
 *   The capacity, in bytes, for the FIFO
 */
size_t cbfifo_capacity(uint8_t rt);

/*
 * Resets the FIFO clearing all elements resulting in a 0 length. Basically,
 * the length of the FIFO goes to 0 after reset but capacity and max_capacity
 * stay intact.
 *
 * Parameters:
 *   rt  Receive (0) or trasmit (1) queue
 *
 * Returns:
 *   Nothing
 */
void   cbfifo_reset(uint8_t rt);

// IGNORE FOLLOWING FUNCTIONS FOR ASSIGNMENT 2.
/*
 * Freezes the FIFO. Once FIFO is frozen, enqueue/dequeue operations will not
 * succeed. Instead, they will always return 0.
 *
 * Parameters:
 *   none
 *
 * Returns:
 *   Nothing
 */
//void   cbfifo_freeze();

/*
 * Unfreezes the FIFO. Once FIFO is unfrozen, normal enqueue/dequeue operations can
 * be resumed.
 *
 * Parameters:
 *   none
 *
 * Returns:
 *   Nothing
 */
//void   cbfifo_unfreeze();

/*
 * Check the freeze status of the FIFO.
 *
 * Parameters:
 *   none
 *
 * Returns:
 *   1 if FIFO is frozen, 0 otherwise.
 */
//int   cbfifo_frozen();


#endif // _CBFIFO_H_
