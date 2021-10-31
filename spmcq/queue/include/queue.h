#ifndef __QUEUE_H__
#define __QUEUE_H__

typedef enum {
    QMETHOD_MUTEX = 0,
    QMETHOD_NUM
} QMETHOD;

typedef struct q_node q_node_t;
typedef struct queue queue_t;

queue_t* spmcq_create(int, QMETHOD);

#endif
