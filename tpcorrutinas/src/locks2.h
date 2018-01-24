
/****** LOCKS2 HEADER ******/

#ifndef __LOCKS_H
#define __LOCKS_H

#include <stdlib.h>
#include "pqueue.h"


typedef struct {
    long long key;
    int state;
    pqueue *queue;
} task_lock_t;


#define TASK_UNLOCKED 0

#define TASK_LOCKED 1

#define TASK_LOCK_IN 1

#define TASK_LOCK_FREE 0

#define TASK_LOCK_INIT ((task_lock_t){TASK_UNLOCKED,TASK_LOCK_FREE,NULL})


void task_lock_l(task_lock_t *);

void task_lock_u(task_lock_t *);

void task_lock_destroy(task_lock_t *t);



#endif //__LOCKS_H

