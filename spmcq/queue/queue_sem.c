#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "queue.h"
#include "queue_internal.h"
#include "queue_sem.h"
#ifdef TEST
#   include "test.h"
#endif

static int qsem_init(queue_t *);
static int qsem_destroy(queue_t *);
static int qsem_enqueue(queue_t *, int);
static int qsem_dequeue(queue_t *, int *);

q_method_t queue_sem_method  = {
    .init = qsem_init,
    .destroy = qsem_destroy,
    .enqueue = qsem_enqueue,
    .dequeue = qsem_dequeue
};

/*
 * TODO: Debug messages for mutex and semaphore
 */
static int qsem_init(queue_t *q)
{
    qsem_lock_t *lock;
    int ret;
    uint8_t lock_succ = 0;

    if (!q) {
        return SPMCQ_INVALID_PARAM;
    }

    if (!IS_POWER_OF_2(q->size)) {
        return SPMCQ_INVALID_PARAM;
    }

    lock = (qsem_lock_t *)calloc(1, sizeof(qsem_lock_t));
    if (!lock) {
        ret = SPMCQ_NO_MEM;
        goto err;
    }

    lock->m = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (!lock->m) {
        ret = SPMCQ_NO_MEM;
        goto err;
    }

    lock->snodes = (sem_t *)malloc(sizeof(sem_t));
    if (!lock->snodes) {
        ret = SPMCQ_NO_MEM;
        goto err;
    }

    lock->sspace = (sem_t *)malloc(sizeof(sem_t));
    if (!lock->sspace) {
        ret = SPMCQ_NO_MEM;
        goto err;
    }

    if (pthread_mutex_init(lock->m, NULL) == 0) {
        lock_succ |= 0x01;
    } else {
        ret = SPMCQ_MUTEX_ERR;
        goto err;
    }

    if (sem_init(lock->snodes, 0, 0) == 0) {
        lock_succ |= 0x02;
    } else {
        ret = SPMCQ_SEMAPHORE_ERR;
        goto err;
    }

    if (sem_init(lock->sspace, 0, q->size) == 0) {
        lock_succ |= 0x04;
    } else {
        ret = SPMCQ_SEMAPHORE_ERR;
        goto err;
    }

    q->lock = lock;

    return SPMCQ_SUCCESS;

err:
    if (lock_succ & 0x01) pthread_mutex_destroy(lock->m);
    if (lock_succ & 0x02) sem_destroy(lock->snodes);
    if (lock_succ & 0x04) sem_destroy(lock->sspace);

    if (lock->m)      free(lock->m);
    if (lock->snodes) free(lock->snodes);
    if (lock->sspace) free(lock->sspace);
    if (lock)         free(lock);

    return ret;
}

/*
 * TODO: Debug messages for mutex and semaphore
 */
static int qsem_destroy(queue_t *q)
{
    int ret = SPMCQ_SUCCESS;
    qsem_lock_t *lock = NULL;

    if (!q) {
        goto err;
    }

    lock = (qsem_lock_t *)q->lock;
    if (!lock) {
        goto err;
    }

    if (lock->m && pthread_mutex_destroy(lock->m) != 0)
        ret = SPMCQ_MUTEX_ERR;

    if (lock->snodes && sem_destroy(lock->snodes) == -1)
        ret = SPMCQ_SEMAPHORE_ERR;

    if (lock->sspace && sem_destroy(lock->sspace) == -1)
        ret = SPMCQ_SEMAPHORE_ERR;

err:
    if (lock) {
        if (lock->m)      free(lock->m);
        if (lock->snodes) free(lock->snodes);
        if (lock->sspace) free(lock->sspace);
        if (lock)         free(lock);
    }

    return ret;
}

/*
 * TODO: Debug messages for mutex and semaphore
 */
static int qsem_enqueue(queue_t *q, int val)
{
    q_node_t *node = NULL;
    qsem_lock_t *lock;
    int ret;

    if (!q) {
        ret = SPMCQ_INVALID_PARAM;
        goto err;
    }

    assert(q->lock);
    lock = q->lock;

    node = (q_node_t *)malloc(sizeof(*node));
    if (!node) {
        ret = SPMCQ_NO_MEM;
        goto err;
    }

    node->val = val;

    if (sem_wait(lock->sspace) != 0) {
        ret = SPMCQ_SEMAPHORE_ERR;
        goto err;
    }

    if (pthread_mutex_lock(lock->m) != 0) {
        ret = SPMCQ_MUTEX_ERR;
        goto err;
    }

    q->ring_buf[(q->front++) & (q->size - 1)] = node;
    q->num++;

    if (pthread_mutex_unlock(lock->m) != 0) {
        ret = SPMCQ_MUTEX_ERR;
        goto err;
    }

    if (sem_post(lock->snodes) != 0) {
        ret = SPMCQ_SEMAPHORE_ERR;
        goto err;
    }

    return SPMCQ_SUCCESS;
err:
    if (node) free(node);

    return ret;
}

/*
 * TODO: Debug messages for mutex and semaphore
 */
static int qsem_dequeue(queue_t *q, int *val)
{
    q_node_t *tmp_node = NULL;
    qsem_lock_t *lock;
    int ret = SPMCQ_SUCCESS;

    if (!q) {
        ret = SPMCQ_INVALID_PARAM;
        goto err;
    }

    assert(q->lock);
    lock = q->lock;

    if (sem_wait(lock->snodes) == -1) {
        ret = SPMCQ_SEMAPHORE_ERR;
        goto err;
    }

    if (pthread_mutex_lock(lock->m) != 0) {
        ret = SPMCQ_MUTEX_ERR;
        goto err;
    }

    tmp_node = q->ring_buf[(q->rear++) & (q->size - 1)];
    q->num--;

    if (pthread_mutex_unlock(lock->m) != 0) {
        ret = SPMCQ_MUTEX_ERR;
        goto err;
    }

    if (sem_post(lock->sspace) == -1) {
        ret = SPMCQ_SEMAPHORE_ERR;
        goto err;
    }

    if (val)
        *val = tmp_node->val;

#ifdef TEST
    observed_items[*val]++;
#endif

err:
    if (tmp_node) free(tmp_node);

    return ret;
}
