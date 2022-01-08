#ifndef __QUEUE_INTERNAL_H__
#define __QUEUE_INTERNAL_H__
#include <stdint.h>
#include "arch.h"

typedef struct q_node {
    int val;
} q_node_t;

typedef struct queue {
    void *lock;
    struct q_method *method;
    uint32_t front;
    uint32_t rear;
    /* The size of the queue */
    uint32_t size;
    /* Current number of nodes in queue */
    int num;

#ifdef TEST
    int *observed_items;
    int nObsvItems;
#endif

    q_node_t **ring_buf __attribute__((__aligned__(CACHE_LINE_SIZE)));
} queue_t __attribute__((__aligned__(CACHE_LINE_SIZE)));

typedef int (* init_cb) (queue_t *);
typedef int (* destroy_cb) (queue_t *);
typedef int (* enqueue_cb) (queue_t *, int);
typedef int (* dequeue_cb) (queue_t *, int *);

typedef struct q_method {
    init_cb init;
    destroy_cb destroy;
    enqueue_cb enqueue;
    dequeue_cb dequeue;
} q_method_t;

#endif
