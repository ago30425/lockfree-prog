#ifndef __QUEUE_INTERNAL_H__
#define __QUEUE_INTERNAL_H__

/*enum QMETHOD {
    QMETHOD_MUTEX = 0,
    QMETHOD_NUM
}*/

typedef struct q_node {
    int val;
} q_node_t;

typedef struct queue {
    q_node_t *front;
    q_node_t *rear;
    struct q_method *method;
    int max_size;
    int num;
} queue_t;

typedef int (* enqueue_cb) (queue_t *, q_node_t *);
typedef q_node_t * (* dequeue_cb) (queue_t *);

typedef struct q_method {
    enqueue_cb enqueue;
    dequeue_cb dequeue;
} q_method_t;

#endif
