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
 * @file    llfifo.c
 * @brief
 *
 *
 * @author  Gavin Medley
 * @date    2023-09-20
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include "llfifo.h"

#define ERROR 0xFFFFFFFFU  // Generic ERROR value (unsigned -1)

/*
 * Data structure that holds one element of a linked list
 * 
 * Parameters
 *   value  The value of the element
 *   next  Pointer to a node_t, the next element in the list
 */
typedef struct node_s node_t;
struct node_s 
{
    void *value;
    node_t *next;  // Pointer to next element in list
};

struct llfifo_s 
{
    uint32_t max_capacity;
    uint32_t capacity;
    uint32_t length;
    /* head points to the first node of the linked list. NULL if no capacity. */
    node_t *head;
    /* tail points to last node in the linked list with a non-NULL value. NULL if no capacity. NULL if no values. */
    node_t *tail;
    node_t *prealloc_start;
    size_t prealloc_length;
};
/*
Cases:

Nominal:
head
|
|x|x|o|o|o|
  |
  tail

Capacity but no values:
head
|
|o|o|o|o|o|

tail is NULL

Capacity but no values:
h
|
|o|

tail is NULL

Capacity 1 with value present:
head
|
|x|
|
tail

Full capacity:
head
|
|x|x|x|x|x|
        |
        tail

If no capacity, head is NULL, tail is NULL
*/

/* For debugging, these counts can be made temporarily available in llfifo.h */
size_t malloc_count = 0;
size_t free_count = 0;

/* Counts malloc calls */
void *my_malloc(size_t size)
{
    malloc_count++;
    return malloc(size);
}

/* Counts free calls */
void my_free(void * ptr)
{
    free_count++;
    return free(ptr);
}

/*
 * Basic multiplication implementation using Egyptian Multiplication or the "Russian Peasant" method
 * 
 * Source: https://stackoverflow.com/a/260662
 * 
 * 
 * Parameters
 *   a  Integer > 0
 *   b  Integer > 0
 * 
 * Returns 
 *   a * b
 */
uint32_t multiply_ints(uint32_t a, uint32_t b)
{
    // sum = a * b
    int sum = 0;
    while (a != 0)
    {
        if ((a & 1) != 0)  // If a is not even, then add b
            sum += b;
        b <<= 1;  // Multiply b by 2
        a >>= 1;  // Divide a by 2
    }
    return sum;
}

/*
 * Create a new linked node containing the given item value
 * 
 * Parameters
 *   item  The item with which to create the node
 * 
 * Returns
 *   The newly created node, with its next pointer initialized to NULL
 */
node_t * ll_create_node(void *item)
{
    /* 
    Allocate memory for the new list node. 
    This ensures that the memory is not invalidated when we leave the scope of this function 
    */
    node_t *new_node = (node_t *)my_malloc(sizeof(node_t));
    if (!new_node) {
        return NULL;  // Return NULL if malloc failed
    }
    new_node->value = item;
    new_node->next = NULL;
    return(new_node);
}

/* 
 * Preallocate a linked list with NULL values with efficient use of malloc
 *
 * Parameters
 *   length  The length of the list to create.
 * 
 * Returns
 *   Pointer to the head of the preallocated linked list.
 */
node_t * ll_preallocate(size_t length)
{
    /* Check for valid input */
    if (length < 1)
    {
        return NULL;
    }
    /* Allocate contiguous memory for our empty list */
    uint32_t nbytes = multiply_ints(sizeof(node_t), length);
    node_t *head = (node_t *)my_malloc(nbytes);
    if (!head)
    {
        return NULL;
    }
    /* Count from 0 through length - 2 (e.g. for length=3, count 0, 1) */
    node_t *node_ptr = head;
    for (int i = 0; i < length - 1; i++)
    {
        node_ptr->value = NULL;
        node_ptr->next = (node_ptr + 1);
        node_ptr = node_ptr->next;
    }
    /* And take care of the last node outside of the loop */
    node_ptr->value = NULL;
    node_ptr->next = NULL;  // Set last node to point to NULL
    return head;
}

/*
 * Dellocate a linked list, including preallocated contiguous memory as allocated by ll_preallocate 
 * 
 * Parameters
 *   head  Head of the list
 *   prealloc_start  Pointer to beginning of preallocated memory (as returned by ll_preallocate)
 *   prealloc_length  Length of preallocated list (number of nodes preallocated, as passed to ll_preallocate)
 */
void ll_deallocate(node_t *head, node_t *prealloc_start, size_t prealloc_length)
{
    if (!head)
    {
        /* There appears to be no linked list to free */
        if (prealloc_start)
        {
            /* If we get here, it means there was a preallocated list but it seems to have been lost. This probably
            means something went very wrong. Free any preallocated memory regardless */
            my_free(prealloc_start);
        }
        return;
    }

    /* Make no assumptions about the memory location of list elements but free them one by one */
    node_t *node_ptr = head;
    node_t *prev = NULL, *to_free = NULL;
    /* Check each node and if its location is not in the preallocated region */
    while (node_ptr)
    {
        if (node_ptr < prealloc_start || node_ptr >= (prealloc_start + prealloc_length)) {
            /* Node is outside preallocated memory region so must be dynamically allocated */
            /* Move previous node to node_ptr->next (maintain linkage but skip over node_ptr) */
            to_free = node_ptr;
            node_ptr = node_ptr->next;
            if (prev)
            {
                /* This maintains linkage of the list */
                prev->next = node_ptr;
            }
            
            my_free(to_free);
            /* We just removed a dynamically allocated node, node_ptr is pointing at the node that is now 
            node number node_index. No need to increment anything so continue loop. */
            continue;
        }
        /* This node is part of the preallocated region so we skip it. It will be freed later. */
        prev = node_ptr;
        node_ptr = node_ptr->next;
    }

    /* Once all dynamically created nodes are gone, free the entire preallocated chunk of memory */
    if (prealloc_start)
    {
        my_free(prealloc_start);  // Frees preallocated portion of linked list
        prealloc_start = NULL;
    }
    return;
}

llfifo_t *llfifo_create(int capacity, int max_capacity)
{
    malloc_count = 0;
    free_count = 0;
    /* Check for invalid inputs */
    if (capacity < 0 || max_capacity <= 0 || capacity > max_capacity)
    {
        return NULL;
    }
    /* Preallocate the linked head */
    node_t *ll_head = ll_preallocate(capacity);
    /* Create the FIFO object with given parameters */
    llfifo_t *fifo_ptr = (llfifo_t *)my_malloc(sizeof(llfifo_t));
    llfifo_t fifo = {
        .max_capacity = max_capacity,
        .capacity = capacity,
        .length = 0,      // No data so length is 0
        .head = ll_head,  // Head node, NULL if no capacity
        .tail = NULL,     // Since there is no data yet, the tail is NULL regardless of how much capacity is allocated
        .prealloc_start = ll_head,  // If no initial capacity, this is NULL
        .prealloc_length = capacity
    };
    *fifo_ptr = fifo;

    return fifo_ptr;
}

int llfifo_enqueue(llfifo_t *fifo, void *element)
{
    if (!element || !fifo)
    {
        return ERROR;
    }

    /* Check if any capacity exists in the linked list */
    if (fifo->head)  // Indicates capacity exists (we don't yet know if that capacity is available)
    {
        if (fifo->tail)
        {
            if (fifo->tail->next)
            {
                /* We found a node that's not used. Store the element here. */
                fifo->tail->next->value = element;
                /* Move the tail to the node we just used to store the element value */
                fifo->tail = fifo->tail->next;
                (fifo->length)++;  // Increment length. Capacity does not change
            } else 
            {
                /* We reached the end of the current capacity. */
                if ((fifo->capacity) < (fifo->max_capacity))
                {
                    /* We are under max capacity so create a new node containing the element value */
                    node_t *new_node = ll_create_node(element);
                    if (!new_node)
                    {
                        return ERROR;
                    }

                    /* Link the current tail node to the new node and move the tail to the new node */
                    fifo->tail->next = new_node;
                    fifo->tail = new_node;

                    (fifo->capacity)++;  // Increment capacity
                    (fifo->length)++;  // Increment length
                } else  // We are at max cap
                {
                    /* We are already at max capacity so dequeue a value and try to enqueue again. 
                    We are losing that data. */
                    llfifo_dequeue(fifo); // Pops the oldest value but does nothing with it
                    llfifo_enqueue(fifo, element);  // Recurse and try to enqueue again
                    // NOTE: Do NOT increase length nor capacity
                }
            }
        } else 
        {
            /* Since there are no values but there is capacity, we just write directly to where head points */
            fifo->head->value = element;
            /* Now that we have a value, we assign tail to the last non-NULL value of the list */
            fifo->tail = fifo->head;
            (fifo->length)++;
        }
    } else  // The rare case that will happen at most once. We have no nodes so head and tail are NULL
    {
        /* Indicates the queue currently has zero capacity (no nodes). Let's dynamically allocate a node */
        node_t *new_node = ll_create_node(element);
        fifo->head = new_node;
        fifo->tail = new_node;
        fifo->capacity = 1;  // Set capacity
        fifo->length = 1;  // Set length
    }
    return fifo->length;
}

void *llfifo_dequeue(llfifo_t *fifo)
{
    if (!fifo)
    {
        return NULL;
    }

    if (fifo->head)
    {
        if (fifo->tail)
        {
            /* Dequeue the oldest item in the queue without reducing the capacity */
            void *element = fifo->head->value;

            node_t *old_head = fifo->head;  // Node that is being dequeued (temp var)
            old_head->value = NULL;  // Null out old head node's value too now that it's been retrieved

            if (fifo->tail != fifo->head)
            {
                fifo->head = fifo->head->next; // New head node of the fifo object after dequeue op

                /* Insert the recently dequeued "old_head" node between the tail and the rest of the capacity nodes */
                old_head->next = fifo->tail->next;  // Point dequeued node towards rest of unused capacity
                fifo->tail->next = old_head;
                (fifo->length)--;  // Decrement length
                return element;
            } else
            {
                old_head->value = NULL;  // Null out value of single node and return early without changing the list
                fifo->tail = NULL;  // Set tail to NULL indicating there are no non-NULL values
                (fifo->length)--;  // Decrement length
                return element;
            }
        } else {
            return NULL;
        }
    }

    return NULL;
}

int llfifo_length(llfifo_t *fifo)
{
    /* 
    The notion of a NULL value indicating an "unused" element of a linked list is not general to the 
    linked list (ll) interface so we use a custom function to find the number of non-null values in the list
    */
    if (!fifo)
    {
        return ERROR;
    }
    return fifo->length;
}

int llfifo_capacity(llfifo_t *fifo)
{
    /* This is simply the number of nodes in the linked list so we use the ll API */
    if (!fifo)
    {
        return ERROR;
    }
    return fifo->capacity;
}

int llfifo_max_capacity(llfifo_t *fifo)
{
    /* Max capacity doesn't change so just return the variable value */
    if (!fifo)
    {
        return ERROR;
    }
    return fifo->max_capacity;
}

void llfifo_destroy(llfifo_t *fifo)
{
    /* First free the internal linked list, including the preallocated contiguous chunk of memory */
    ll_deallocate(fifo->head, fifo->prealloc_start, fifo->prealloc_length);
    fifo->head = NULL;
    fifo->tail = NULL;
    fifo->prealloc_start = NULL;
    /* Free the fifo object itself */
    my_free(fifo);
    fifo = NULL;
    return;
}

node_t *llfifo_list(llfifo_t *fifo)
{
    return fifo->head;
}

size_t count_ll_nodes(llfifo_t *fifo)
{
    node_t *head = fifo->head;
    size_t count = 0;
    while (head)
    {
        count++;
        head = head->next;
    }
    return count;
}

size_t count_non_null_ll_nodes(llfifo_t *fifo)
{
    node_t *head = fifo->head;
    size_t count = 0;
    while (head)
    {
        if (head->value)
        {
            count++;
        }
        head = head->next;
    }
    return count;
}