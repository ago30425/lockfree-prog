#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "queue.h"
#include "queue_internal.h"
#include "queue_sem.h"
#ifdef TEST
#   include "test.h"
#endif

queue_t* spmcq_create(int size, QMETHOD method_id)
{
    queue_t *q = NULL;
    q_node_t **ring_buf = NULL;
    q_method_t *m;

    if (size <= 0)
        return NULL;

    switch (method_id) {
    case QMETHOD_SEM:
        m = &queue_sem_method;
        break;
    default:
        m = &queue_sem_method;
        break;
    }

    q = (queue_t *)calloc(1, sizeof(*q));
    if (!q)
        goto err;

    ring_buf = (q_node_t **)malloc(sizeof(q_node_t *) * size);
    if (!ring_buf)
        goto err;

    q->ring_buf = ring_buf;
    q->size = size;
    q->method = m;

    if (q->method->init &&
        q->method->init(q) < 0) {
        goto err;
    }

    return q;

err:
    if (ring_buf) free(ring_buf);
    if (q)        free(q);

    return NULL;
}

void spmcq_release(queue_t *spmcq)
{
    if (!spmcq)
        return;

    assert(spmcq->method);

    if (spmcq->method->destroy)
        spmcq->method->destroy(spmcq);

    if (spmcq->ring_buf) free(spmcq->ring_buf);
    free(spmcq);
}

int spmcq_get_maxsize(queue_t *spmcq)
{
    int size = 0;

    if (spmcq)
        size = spmcq->size;

    return size;
}

int spmcq_enqueue(queue_t *spmcq, int val)
{
    if (!spmcq)
        return SPMCQ_INVALID_PARAM;

    assert(spmcq->method &&
           spmcq->method->enqueue);

    return spmcq->method->enqueue(spmcq, val);
}

int spmcq_dequeue(queue_t *spmcq, int *val)
{
    if (!spmcq)
        return SPMCQ_INVALID_PARAM;

    assert(spmcq->method &&
           spmcq->method->dequeue);

    return spmcq->method->dequeue(spmcq, val);
}

#ifdef TEST
void spmcq_test(queue_t *spmcq)
{
    if (!spmcq) {
        fprintf(stderr, "[Failed] spmcq is NULL\n");
        return;
    }

    printf("SPMC test...\n");
    for (int i = 0; i < spmcq_get_maxsize(spmcq); i++) {
        if (__atomic_load_n(&observed_items[i], __ATOMIC_RELAXED) == 1) {
            continue;
        }

        fprintf(stderr, "[Failed] Item %d has been seen %d times\n",
                i, observed_items[i]);
        return;
    }

    printf("[OK] Manipulation of SPMC queue passed\n");
}
#endif


