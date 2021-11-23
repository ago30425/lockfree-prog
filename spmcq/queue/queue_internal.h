#ifndef __QUEUE_INTERNAL_H__
#define __QUEUE_INTERNAL_H__
#include <stdint.h>

/*enum QMETHOD {
    QMETHOD_SEM = 0,
    QMETHOD_NUM
}*/

typedef struct q_node {
    int val;
} q_node_t;

typedef struct queue {
    void *lock;
    q_node_t **ring_buf;
    /* TODO: Make portable type */
    uint32_t front;
    uint32_t rear;
	struct q_method *method;
    /* Max size of the queue */
    int  max_size;
    /* Current number of nodes in queue */
    int  num;
} queue_t;

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
