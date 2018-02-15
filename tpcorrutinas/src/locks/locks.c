
#include "scheduler.h"
#include "locks.h"
#include <stdio.h>
#include <stdlib.h>


/*
void task_sp_lock(task_sp_t *n)
{
    task_sp_t own_key = TASK_SP_LOCK;
    while (1) {
        asm volatile ("xchgq %0, %1\n"
                      : "+r" (*n), "+r" (own_key));
        if (own_key == TASK_SP_LOCK)
            task_yield();
        else if (own_key == TASK_SP_UNLOCK)
            return;
    }
}
*/


void task_sp_lock(task_sp_t *n)
{
    while (! __sync_bool_compare_and_swap(n, TASK_SP_UNLOCK, TASK_SP_LOCK)) {
    	while (*n == TASK_SP_LOCK)
    		task_yield();
    }
}


void task_sp_unlock(task_sp_t *n)
{
    if (*n == TASK_SP_UNLOCK)
        exit(1);
    *n = TASK_SP_UNLOCK;
    return;
}


