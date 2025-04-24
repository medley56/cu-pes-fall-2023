/**
 * @file cbfifo.h
 * @brief Two fixed-size FIFO queues (Q_TX and Q_RX) backed by circular buffers.
 *
 * This module is intended to provide one read queue and one write queue. The queues have
 * a fixed size of 127 bytes. When a buffer fills, enqueue operations will refuse to enqueue
 * additional data.
 *
 * @author Howdy Pierce, Lalit Pandit, Gavin Medley
 */

#ifndef _CBFIFO_H_
#define _CBFIFO_H_

#include <stdlib.h>  // for size_t
#include <stdint.h>

/* The following constants should be used to pass the qid parameter to cbfifo API functions */
#define Q_RX (0)  // Receive queue selector
#define Q_TX (1)  // Transmit queue selector

/**
 * @brief Initialize and reset the RECEIVE and TRANSMIT queues.
 *
 * This must be called before attempting to use the FIFO queues. It is idempotent so calling it
 * multiple times only has the effect of resetting the queues.
 */
void cbfifo_init();

/**
 * @brief Enqueues data onto the specified FIFO, up to the limit of the available FIFO capacity.
 *
 * This function enqueues data onto either the receive (0) or transmit (1) queue, depending on the
 * value of `qid`. It attempts to enqueue `nbyte` bytes of data from the buffer pointed to by `buf`.
 * However, the actual number of bytes enqueued may be less if the FIFO does not have enough capacity.
 *
 * @param qid    Receive (0) or transmit (1) queue.
 * @param buf   Pointer to the data to be enqueued.
 * @param nbyte Max number of bytes to enqueue.
 * @return The number of bytes actually enqueued, which could be 0. In case of an error, returns
 *         (size_t) -1. For example, if the FIFO has a capacity of 30 bytes and is currently empty,
 *         a call to enqueue 35 bytes will enqueue only 30 bytes and return 30. The application
 *         should attempt to enqueue any remaining bytes subsequently.
 */
size_t cbfifo_enqueue(uint8_t qid, void *buf, size_t nbyte);

/**
 * @brief Attempts to remove ("dequeue") up to `nbyte` bytes of data from the FIFO.
 *
 * Dequeued data will be copied into the buffer pointed to by `buf`. If the FIFO's current length
 * is less than the requested bytes, only the available bytes are copied, and the FIFO length
 * becomes 0. If the FIFO is empty, the function returns 0.
 *
 * @param qid    Receive (0) or transmit (1) queue.
 * @param buf   Destination for the dequeued data.
 * @param nbyte Bytes of data requested.
 * @return The number of bytes actually copied, which will be between 0 and nbyte (inclusive).
 */

size_t cbfifo_dequeue(uint8_t qid, void *buf, size_t nbyte);

/**
 * @brief Returns the number of bytes currently on the FIFO.
 *
 * This function provides the current count of bytes in the FIFO, indicating how much data
 * is available to be dequeued.
 *
 * @param qid Receive (0) or transmit (1) queue.
 * @return Number of bytes currently available to be dequeued from the FIFO.
 */
size_t cbfifo_length(uint8_t qid);

/**
 * @brief Returns the FIFO's capacity.
 *
 * This function provides the total capacity of the FIFO in bytes.
 *
 * @param qid Receive (0) or transmit (1) queue.
 * @return The capacity, in bytes, of the FIFO.
 */
size_t cbfifo_capacity(uint8_t qid);

/**
 * @brief Resets the FIFO, clearing all elements and setting its length to 0.
 *
 * This function resets the FIFO, effectively clearing its contents. After the reset,
 * the length of the FIFO will be 0, but its capacity and max_capacity remain unchanged.
 *
 * @param qid Receive (0) or transmit (1) queue.
 */
void cbfifo_reset(uint8_t qid);

#endif // _CBFIFO_H_
