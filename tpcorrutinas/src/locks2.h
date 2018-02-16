
/****** LOCKS2 HEADER ******/

#ifndef __LOCKS_H
#define __LOCKS_H

#include <stdlib.h>
#include "pqueue.h"


typedef struct {
    int key;
    pqueue *queue;
} task_lock_t;


#define TASK_LOCK_INIT ((task_lock_t){1,NULL})


void task_q_lock(task_lock_t *);

void task_q_unlock(task_lock_t *);

void task_lock_destroy(task_lock_t *t);



#endif //__LOCKS_H

