#ifndef __QUEUE_INTERNAL_H__
#define __QUEUE_INTERNAL_H__
#include <stdint.h>
#include "arch.h"

#define QUEUE_OFFSET(idx, q)        ((idx) & ((q)->mask))
#define QUEUE_FRONT_ATOMIC(q)       (__atomic_load_n(&(q)->front, __ATOMIC_RELAXED))
#define QUEUE_REAR_ATOMIC(q)        (__atomic_load_n(&(q)->rear, __ATOMIC_RELAXED))
/* Be careful to use macro below
 * as it just simple ways to get information from queue.
 * Might not be suitable for lock-free operation.
 */
#define QUEUE_FRONT(q)              (&(q)-front)
#define QUEUE_REAR(q)               (&(q)->rear)
#define QUEUE_LEN(q)                (QUEUE_FRONT(q) - QUEUE_REAR(q))
#define QUEUE_FULL(q)               (QUEUE_LEN(q) == (q)->size)
#define QUEUE_EMPTY(q)              ((q)->front == (q)->rear)

typedef struct q_node {
    int val;
} q_node_t;

typedef struct queue {
    void *lock;
    struct q_method *method;
    volatile uint32_t front, rear;
    uint32_t mask;
    /* The size of the queue */
    uint32_t size;

#ifdef TEST
    int *observed_items;
    int nObsvItems;
#endif

    // TODO: use arrays of length zero
    q_node_t **ring_buf __attribute__((__aligned__(CACHE_LINE_SIZE)));
} queue_t __attribute__((__aligned__(CACHE_LINE_SIZE)));

typedef int (* init_cb) (queue_t *);
typedef int (* destroy_cb) (queue_t *);
typedef int (* enqueue_cb) (queue_t *, int);
typedef int (* dequeue_cb) (queue_t *, int *);

/* The enqueue and dequeue are blocking operation.
 */
typedef struct q_method {
    init_cb init;
    destroy_cb destroy;
    enqueue_cb enqueue;
    dequeue_cb dequeue;
} q_method_t;

#endif
