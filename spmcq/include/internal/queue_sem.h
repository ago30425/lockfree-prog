#ifndef __QUEUE_SEM_H__
#define __QUEUE_SEM_H__
#include <pthread.h>
#include <semaphore.h>

typedef struct {
    pthread_mutex_t *m;
    sem_t *snodes;
    sem_t *sspace;
} qsem_lock_t;

extern q_method_t queue_sem_method;


#endif
