#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define _DBG_THREAD_INFO
//#define _DBG_CMD_PARSING

#ifdef _DBG_THREAD_INFO
static inline void thread_info_print(pthread_t thr_id, const char *thr_name)
{
    int ret;
    cpu_set_t cpuset;

    ret = pthread_getaffinity_np(thr_id, sizeof(cpuset), &cpuset);
    if (ret != 0) {
        fprintf(stderr, "pthread_getaffinity_np failed");
        return;
    }

    printf("This is thread (%s) can be run on cpu: ", thr_name);
    for (int i = 0; i < CPU_SETSIZE; i++) {
        if (CPU_ISSET(i, &cpuset))
            printf("%d ", i);
    }
    printf("\n");
}
#else
#   define thread_info_print(...)
#endif

#ifdef _DBG_CMD_PARSING
#   define DBG_CMD_PARSING(fmt, ...)    printf(fmt, ##__VA_ARGS__);
#else
#   define DBG_CMD_PARSING(...)
#endif

#endif
