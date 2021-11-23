#ifndef _GNU_SOURCE
#   define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "debug.h"
#include "error.h"
#include "queue.h"

static void * producer_thread(void *args)
{
    int i;
    queue_t *spmcq = (queue_t *)args;

    for (i = 0; i < spmcq_get_maxsize(spmcq); i++) {
        /* TODO: error handling */
        spmcq_enqueue(spmcq, i);
    }

    return NULL;
}

static void * consumer_thread(void *args)
{
    int val;
    queue_t *spmcq = (queue_t *)args;
    int max_items = spmcq_get_maxsize(spmcq);

    /* TODO: error handling */
    for (;;) {
        spmcq_dequeue(spmcq, &val);

        /* Notify other consumers to stop */
        if (val >= max_items - 1) {
            spmcq_enqueue(spmcq, max_items);
            break;
        }
    }

    return NULL;
}

static inline int pin_thread_to_cpu(pthread_t thr_id, int cpu_id)
{
    int ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    cpu_set_t cpuset;

    if (cpu_id < 0)
        return EINVAL;

    cpu_id %= ncpu;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_id, &cpuset);

    return pthread_setaffinity_np(thr_id, sizeof(cpu_set_t), &cpuset);
}

static double time_diff_ms(struct timespec start, struct timespec end)
{
    struct timespec ret;

    if (start.tv_nsec > end.tv_nsec) {
        ret.tv_sec = end.tv_sec - start.tv_sec - 1;
        ret.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    } else {
        ret.tv_sec = end.tv_sec - start.tv_sec;
        ret.tv_nsec = end.tv_nsec - start.tv_nsec;
    }

    return ((ret.tv_sec * 1000.0) + (ret.tv_nsec / 1000000.0));
}

int main(int argc, char *argv[])
{
    /* Parse command */
    if (argc < 3) {					// TODO
        ERR_HANDLE_PRINT("spmcq <thread number> <queue size> <test ID>\n");
    }

    int i, ret;
    int nthread = atoi(argv[1]);	// FIXME
    int qsize = atoi(argv[2]);      // FIXME: check max size and power of 2
    int testId = atoi(argv[3]);		// FIXME
    struct timespec start_time, end_time;
    pthread_t prod_tid, *cons_tids;
    queue_t* spmcq = spmcq_create(qsize, testId);

    if (!spmcq) {
        ERR_HANDLE_PRINT("spmcq_create failed\n");
    }

    /* Create threads and do CPU pinning*/
    cons_tids = (pthread_t *)malloc(sizeof(pthread_t) * (nthread - 1));
    if (!cons_tids) {
        ERR_HANDLE_PRINT("malloc failed\n");
    }

    ret = pthread_create(&prod_tid, NULL, producer_thread, (void *)spmcq);
    if (ret != 0) {
        ERR_HANDLE_EX("pthread_create", ret);
    }

    ret = pin_thread_to_cpu(prod_tid, 0);
    if (ret != 0) {
        ERR_HANDLE_EX("pthread_setaffinity_np", ret);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &start_time) != 0) {
        ERR_HANDLE("clock_gettime");
    }

    for (i = 0; i < nthread - 1; i++) {
        ret = pthread_create(&cons_tids[i], NULL, consumer_thread, (void *)spmcq);
        if (ret != 0) {
            ERR_HANDLE_EX("pthread_create", ret);
        }

        ret = pin_thread_to_cpu(cons_tids[i], i + 1);
        if (ret != 0) {
            ERR_HANDLE_EX("pthread_setaffinity_np", ret);
        }
    }

    /* Join threads */
    ret = pthread_join(prod_tid, NULL);
    if (ret != 0) {
        ERR_HANDLE_EX("pthread_join", ret);
    }

    for (i = 0; i < nthread - 1; i++) {
        ret = pthread_join(cons_tids[i], NULL);
        if (ret != 0) {
            ERR_HANDLE_EX("pthread_join", ret);
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end_time) != 0) {
        ERR_HANDLE("clock_gettime");
    }

    /* Execution time */
    printf("Execution time: %f ms\n", time_diff_ms(start_time, end_time));

    /* Test */
    spmcq_test(spmcq);

    /* End */
    if (cons_tids)
        free(cons_tids);

    spmcq_release(spmcq);

    return 0;
}
