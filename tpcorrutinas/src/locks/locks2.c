
#include "locks2.h"
#include "pqueue.h"
#include "scheduler.h"
#include <malloc.h>
#include <unistd.h>

#define debug(n) (write(STDOUT_FILENO,"debug "n"\n",8))


void task_lock_destroy(task_lock_t *l)
{
    block_sched();
    if (l -> queue != NULL)
        queue_destroy(l->queue);
    unblock_sched();
    return;
}


void task_lock_l(task_lock_t *l)
{
    long long own_key = TASK_LOCKED;
    while (1) {
        l -> state = TASK_LOCK_IN;
        //printf("Before: l->key = %lld, own = %lld\n",l->key,own_key);
        asm("movq %0, %%r12\n"
            "xchg %%r12, %1\n"
            "movq %%r12, %0"
            : "+r" (own_key), "+g" (l->key));
        //printf("After: l->key = %lld, own = %lld\n",l->key,own_key);
        if (own_key == TASK_UNLOCKED) {
            //debug("e");
            l -> state = TASK_LOCK_FREE;
            return;
        }
        else {
            //debug("f");
            block_sched();
            if (l -> queue == NULL)
                l -> queue = queue_create(1);
            queue_new_node(l->queue, task_current());
            task_state(BLOCKED);
            unblock_sched();
            l -> state = TASK_LOCK_FREE;
            task_yield();
        }
    }
}


void task_lock_u(task_lock_t *l)
{
    while (1) {
        //debug("a");
        block_sched();
        if (l -> state == TASK_LOCK_IN) {
            //debug("b");
            unblock_sched();
            task_yield();
        }
        else
            break;
    }
    if (l->queue != NULL) {
        //debug("c");
        q_elem *wakeup = queue_pop(l->queue);
        if (wakeup != NULL) {
            unblock_task((Task *) wakeup -> data);
            free(wakeup);
        }
    }
    //debug("d");
    l -> key = TASK_UNLOCKED;
    unblock_sched();
    task_yield();
}


