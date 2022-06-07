#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <spmcq/spmcq.h>
#include "queue_internal.h"
#include "queue_lockfree.h"
#include "compiler.h"

static int qlockfree_init(queue_t *q);
static int qlockfree_destroy(queue_t *q);
static int qlockfree_enqueue(queue_t *q, int val);
static int qlockfree_dequeue(queue_t *q, int *val);

q_method_t queue_lockfree_method  = {
     /* All necessary initialization and release
      * are done in spmcq APIs.
      */
    .init = NULL,
    .destroy = NULL,
    .enqueue = qlockfree_enqueue,
    .dequeue = qlockfree_dequeue
};

static ATTRIBUTE(unused) int qlockfree_init(queue_t *q)
{
    if (!q) {
        return SPMCQ_INVALID_PARAM;
    }

    if (!IS_POWER_OF_2(q->size)) {
        return SPMCQ_INVALID_PARAM;
    }

    return SPMCQ_SUCCESS;
}

static ATTRIBUTE(unused) int qlockfree_destroy(queue_t *q)
{
    /* Suppress compiler warning message. */
    if (!q)
        return SPMCQ_SUCCESS;

    return SPMCQ_SUCCESS;
}

static int qlockfree_enqueue(queue_t *q, int val)
{
    q_node_t *node = NULL;
    int ret;

    if (!q) {
        ret = SPMCQ_INVALID_PARAM;
        goto err;
    }

    node = (q_node_t *)malloc(sizeof(*node));
    if (!node) {
        ret = SPMCQ_NO_MEM;
        goto err;
    }

    node->val = val;

    while (q->rear - QUEUE_FRONT_ATOMIC(q)
           == q->size);

    q->ring_buf[QUEUE_OFFSET(q->rear, q)] = node;
    __atomic_add_fetch(&q->rear, 1, __ATOMIC_RELEASE);

    return SPMCQ_SUCCESS;
err:
    if (node)
        free(node);

    return ret;
}

static int qlockfree_dequeue(queue_t *q, int *val)
{
    q_node_t *tmp_node = NULL;
    uint32_t tmp_front;
    int ret = SPMCQ_SUCCESS;

    if (!q) {
        ret = SPMCQ_INVALID_PARAM;
        goto err;
    }

    do {
retry:
        tmp_front = QUEUE_FRONT_ATOMIC(q);
        if (tmp_front == q->rear)
            goto retry;

        tmp_node = q->ring_buf[QUEUE_OFFSET(tmp_front, q)];
        /* No ABA problem here because front is 32-bit long and
         * it takes a long time to wrap around if the thread is preempted before CAS.
         */
    } while (!__atomic_compare_exchange_n(&q->front, &tmp_front, tmp_front + 1,
                                          1, __ATOMIC_RELAXED, __ATOMIC_RELAXED));

    if (val)
        *val = tmp_node->val;

#ifdef TEST
    __atomic_add_fetch(&q->observed_items[*val], 1, __ATOMIC_RELAXED);
#endif

err:
    if (tmp_node)
        free(tmp_node);

    return ret;
}

