
#include "scheduler.h"
#include "locks.h"
#include <stdio.h>

#include <unistd.h>

#define debug(n) (write(STDOUT_FILENO,"debug "n"\n",8))


void task_sp_lock(task_sp_t *n)
{
    task_sp_t own_key = TASK_SP_LOCK;
    while (1) {
        //asm("mfence");
        asm("xchgq %0, %1"
            : "+r" (*n), "+r" (own_key));
        if (own_key == TASK_SP_LOCK)
            task_yield();
        else if (own_key == TASK_SP_UNLOCK) {
            //debug("a");
            break;
        }
    }
    return;
}


void task_sp_unlock(task_sp_t *n)
{
    //debug("b");
    *n = TASK_SP_UNLOCK;
    return;
}


