#include <stdio.h>
#include "queue_internal.h"

q_method_t queue_mutex_method  = {
    .enqueue = NULL,
    .dequeue = NULL
};


