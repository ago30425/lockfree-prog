#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/queue.h"
#include "queue_internal.h"
#include "queue_mutex.h"

queue_t* spmcq_create(int max_size, QMETHOD method_id)
{
    queue_t *q;
    q_method_t *m;

    if (max_size == 0)
        return NULL;
    
    switch (method_id) {
        case QMETHOD_MUTEX:
            m = &queue_mutex_method;
            break;
        default:
            m = &queue_mutex_method;
            break;
    }

    q = (queue_t *)malloc(sizeof(*q));
    if (!q)
        return NULL;

    memset(q, 0, sizeof(*q));
    q->max_size = max_size;
    q->method = m;

    return q;
}
