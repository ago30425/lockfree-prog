#ifndef __SPMCQ_H__
#define __SPMCQ_H__
#include <limits.h>
#include <stdint.h>

#define MIN_QUEUE_SIZE    1
#define MAX_QUEUE_SIZE    (1 << 12)  // 4 KB

#define IS_POWER_OF_2(x) ((((x) - 1) & (x)) == 0)

enum SPMCQ_ERR {
    SPMCQ_SUCCESS = 0,
    SPMCQ_NO_MEM = -20,
    SPMCQ_SEMAPHORE_ERR,
    SPMCQ_MUTEX_ERR,
    SPMCQ_INVALID_PARAM
};

typedef enum {
    QMETHOD_SEM = 0,
    QMETHOD_LOCKFREE,
    QMETHOD_NUM
} QMETHOD;

typedef struct q_node q_node_t;
typedef struct queue queue_t;

#ifdef TEST
queue_t* spmcq_create(uint32_t size, QMETHOD method_id, int nData);
#else
queue_t* spmcq_create(uint32_t size, QMETHOD method_id);
#endif
uint32_t spmcq_get_maxsize(queue_t *q);
int spmcq_enqueue(queue_t *q, int val);
int spmcq_dequeue(queue_t *q, int *val);
void spmcq_release(queue_t *q);

#ifdef TEST
void spmcq_test(queue_t *q);
#else
#define spmcq_test(...)
#endif

#endif
