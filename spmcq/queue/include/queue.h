#ifndef __QUEUE_H__
#define __QUEUE_H__
#include <limits.h>

#define MAX_QUEUE_SIZE    (INT_MAX - 1)

enum SPMCQ_ERR {
    SPMCQ_SUCCESS = 0,
    SPMCQ_NO_MEM = -20,
    SPMCQ_SEMAPHORE_ERR,
    SPMCQ_MUTEX_ERR,
    SPMCQ_INVALID_PARAM
};

typedef enum {
    QMETHOD_SEM = 0,
    QMETHOD_NUM
} QMETHOD;

typedef struct q_node q_node_t;
typedef struct queue queue_t;

queue_t* spmcq_create(int, QMETHOD);
int spmcq_get_maxsize(queue_t *);
int spmcq_enqueue(queue_t *, int);
int spmcq_dequeue(queue_t *, int *);
void spmcq_release(queue_t *);

#ifdef TEST
void spmcq_test(queue_t *);
#else
#define spmcq_test(...)
#endif

#endif
