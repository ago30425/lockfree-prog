#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define ERR_HANDLE_EX(MSG, EN)	\
	do { errno = EN; perror(MSG); exit(EXIT_FAILURE); } while (0);

#define ERR_HANDLE(MSG)  \
	do { perror(MSG); exit(EXIT_FAILURE); } while (0);

#define ERR_HANDLE_PRINT(FMT, ...)	\
	do { fprintf(stderr, FMT, ##__VA_ARGS__); exit(EXIT_FAILURE); } while (0);

static inline void thread_info_print(pthread_t thr_id, const char *thr_name)
{
	int ret;
    cpu_set_t cpuset;

	ret = pthread_getaffinity_np(thr_id, sizeof(cpuset), &cpuset);
    if (ret != 0)
    	ERR_HANDLE_EX("pthread_getaffinity_np", ret);

    printf("This is thread: %s", thr_name);
    for (int i = 0; i < CPU_SETSIZE; i++) {
    	if (CPU_ISSET(i, &cpuset))
        	printf(", CPU ID: %d", i);
	}
	printf("\n");
}

static void * producer_thread(void *args)
{
	
	return NULL;
}

static void * consumer_thread(void *args)
{
	
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

int main(int argc, char *argv[])
{
	/* Parse command */
	if (argc < 3) {					// TODO
		ERR_HANDLE_PRINT("spmcq <thread number> <test ID>\n");
	}

	int i, ret;
	int nthread = atoi(argv[1]);	// FIXME
	int testId = atoi(argv[2]);		// FIXME
	pthread_t prod_tid, *cons_tids;

	cons_tids = (pthread_t *)malloc(sizeof(pthread_t) * (nthread - 1));
	if (!cons_tids) {
		ERR_HANDLE_PRINT("[%s %d] malloc failed\n", __func__, __LINE__);
	}

	/* Create threads and do CPU pinning*/
	ret = pthread_create(&prod_tid, NULL, producer_thread, NULL);
	if (ret != 0) {
		ERR_HANDLE_EX("pthread_create", ret);
	}

	ret = pin_thread_to_cpu(prod_tid, 0);
	if (ret != 0) {
		ERR_HANDLE_EX("pthread_setaffinity_np", ret);
	}

	for (i = 0; i < nthread - 1; i++) {
		ret = pthread_create(&cons_tids[i], NULL, consumer_thread, NULL);
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

	/* End */
	if (cons_tids)
		free(cons_tids);

    return 0;
}
