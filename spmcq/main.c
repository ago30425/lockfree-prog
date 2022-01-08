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

/****************** Command *****************/
#define CMD_OPT_NBTHREADS    'n'
#define CMD_OPT_QSIZE        'q'
#define CMD_OPT_METHOD       'i'
#define CMD_OPT_HELP         'h'
#define CND_OPT_DUMMY        0xFF

typedef struct {
    int opt;
    const char *help_msg;
} cmd_opts_t;

/* TODO: flag for disabling CPU affinity*/
cmd_opts_t cmdOptTbl[] = {
    {CMD_OPT_NBTHREADS,  "Specify the number of threads"},
    {CMD_OPT_QSIZE,      "Specify the size of queue"},
    {CMD_OPT_METHOD,     "Specify which method to use to avoid data race"},
    {CMD_OPT_HELP,       "Show help messages"},
    {CND_OPT_DUMMY,      NULL}
};

/****************** Config ******************/
#define MIN_THREAD_NUM       2
#define MAX_THREAD_NUM       128

typedef struct {
    long nthread;
    long qsize;
    long testId;
} config_t;

static void * producer_thread(void *args)
{
    queue_t *spmcq = (queue_t *)args;

    for (uint32_t i = 0; i < spmcq_get_maxsize(spmcq); i++) {
        /* TODO: error handling */
        spmcq_enqueue(spmcq, (int)i);
    }

    return NULL;
}

static void * consumer_thread(void *args)
{
    int val;
    queue_t *spmcq = (queue_t *)args;
    uint32_t max_items = spmcq_get_maxsize(spmcq);

    /* TODO: error handling */
    for (;;) {
        spmcq_dequeue(spmcq, &val);

        /* Notify other consumers to stop */
        if ((uint32_t)val >= max_items - 1) {
            spmcq_enqueue(spmcq, (int)max_items);
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

static void print_help_msg(void)
{
    int i;

    printf("Usage: spmcq [OPTIONS]...\n");
    printf("OPTIONS\n");

    for (i = 0; cmdOptTbl[i].help_msg; i++) {
        printf("\t-%c:\t\t%s\n", cmdOptTbl[i].opt, cmdOptTbl[i].help_msg);
    }
}

static void config_set_default(config_t *cfg)
{
    if (cfg->nthread == 0) {
        cfg->nthread = 16;
    }

    if (cfg->qsize == 0) {
        cfg->qsize = 1024;
    }
}

int main(int argc, char *argv[])
{
    int i, ret;
    int opt;
    config_t configs = {0};
    struct timespec start_time, end_time;
    pthread_t prod_tid, *cons_tids;
    queue_t* spmcq;

    /* Parse command */
    while ((opt = getopt(argc, argv, "n:q:i:h")) != -1) {
        switch (opt) {
        case CMD_OPT_NBTHREADS:
            configs.nthread = strtol(optarg, NULL, 10);
            CHECK_NUM_OUT_OF_RANGE(opt, configs.nthread, MIN_THREAD_NUM, MAX_THREAD_NUM);
            break;
        case CMD_OPT_QSIZE:
            configs.qsize = strtol(optarg, NULL, 10);
            CHECK_NUM_OUT_OF_RANGE(opt, configs.qsize, MIN_QUEUE_SIZE, MAX_QUEUE_SIZE);
            CHECK_NUM_IS_POWER_OF_2(opt, configs.qsize);
            break;
        case CMD_OPT_METHOD:
            configs.testId = strtol(optarg, NULL, 10);
            CHECK_NUM_OUT_OF_RANGE(opt, configs.testId, QMETHOD_SEM, QMETHOD_NUM - 1);
            break;
        case CMD_OPT_HELP:
            print_help_msg();
            exit(EXIT_SUCCESS);
        default:
            print_help_msg();
            exit(EXIT_FAILURE);
        }
    }

    DBG_CMD_PARSING("Command options: optind: %d, nthread: %d, qsize: %d, testId: %d\n",
                    optind, (int)configs.nthread, (int)configs.qsize, (int)configs.testId);

    if (argc - 1 >= optind) {
        ERR_HANDLE_PRINT("Too many arguments\n");
    }

    config_set_default(&configs);

    DBG_CMD_PARSING("Configs: nthread: %d, qsize: %d, testId: %d\n",
                    (int)configs.nthread, (int)configs.qsize, (int)configs.testId);


    spmcq =
#ifdef TEST
        spmcq_create((uint32_t)configs.qsize, configs.testId, (int)configs.nData);
#else
        spmcq_create((uint32_t)configs.qsize, configs.testId);
#endif

    if (!spmcq) {
        ERR_HANDLE_PRINT("spmcq_create failed\n");
    }

    /* Create threads and do CPU pinning*/
    cons_tids = (pthread_t *)malloc(sizeof(pthread_t) * (configs.nthread - 1));
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

    for (i = 0; i < configs.nthread - 1; i++) {
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

    for (i = 0; i < configs.nthread - 1; i++) {
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
