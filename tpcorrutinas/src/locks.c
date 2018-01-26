
#include "scheduler.h"
#include "locks.h"
#include <stdio.h>


void task_sp_lock(task_sp_t *n)
{
    task_sp_t own_key = TASK_SP_LOCK;
    while (1) {
        asm("mfence\n"
            "xchgq %0, %1\n"
            : "+r" (*n), "+r" (own_key));
        if (own_key == TASK_SP_LOCK)
            task_yield();
        else if (own_key == TASK_SP_UNLOCK)
            return;
    }
}


void task_sp_unlock(task_sp_t *n)
{
    asm("mfence");
    *n = TASK_SP_UNLOCK;
    return;
}


