#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "queue.h"
#include "queue_internal.h"
#include "queue_sem.h"
#include "queue_lockfree.h"
#ifdef TEST
#   include "test.h"
#endif

#ifdef TEST
queue_t* spmcq_create(uint32_t size, QMETHOD method_id, int nData)
#else
queue_t* spmcq_create(uint32_t size, QMETHOD method_id)
#endif
{
    queue_t *q = NULL;
    q_node_t **ring_buf = NULL;
    q_method_t *m;
#ifdef TEST
    int *observed_items = NULL;
#endif

    if (size == 0 || !IS_POWER_OF_2(size))
        return NULL;

    switch (method_id) {
    case QMETHOD_SEM:
        m = &queue_sem_method;
        break;
    case QMETHOD_LOCKFREE:
        m = &queue_lockfree_method;
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
    q->mask = size - 1;

#ifdef TEST
    q->nObsvItems = nData + 1;
    observed_items = (int *)calloc(1, sizeof(int) * q->nObsvItems);
    if (!observed_items)
        goto err;

    q->observed_items = observed_items;
#endif

    if (q->method->init &&
        q->method->init(q) < 0) {
        goto err;
    }

    return q;

err:
    if (ring_buf) free(ring_buf);
    if (q)        free(q);
#ifdef TEST
    if (observed_items) free(observed_items);
#endif

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

uint32_t spmcq_get_maxsize(queue_t *spmcq)
{
    uint32_t size = 0;

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
        printf("[Failed] spmcq is NULL\n");
        return;
    }

    printf("SPMC test...\n");
    for (int i = 0; i < spmcq->nObsvItems - 1; i++) {
        if (spmcq->observed_items[i] == 1) {
            continue;
        }

        printf("[Failed] Item %d has been seen %d times\n",
               i, spmcq->observed_items[i]);
        return;
    }

    printf("[OK] Manipulation of SPMC queue passed\n");
}
#endif


