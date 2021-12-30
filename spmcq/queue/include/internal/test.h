#ifndef __TEST_H__
#define __TEST_H__
#include "queue.h"

/* TODO: Portable atomic type */
_Atomic int observed_items[MAX_QUEUE_SIZE + 1];

#endif
