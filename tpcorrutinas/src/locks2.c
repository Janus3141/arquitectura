
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


void task_q_blocker(task_lock_t *l)
{
    while (l->key == 0) {
        block_sched();
        if (l -> queue == NULL)
            l -> queue = queue_create(1);
        queue_new_node(l->queue, task_current());
        task_state(BLOCKED);
        unblock_sched();
        task_yield();
    }
    return;
}


void task_q_lock(task_lock_t *l)
{
    asm("movq %0, %%rbx\n"
        "trylock:\n"
        "movl $1, %%eax\n"
        "movl $0, %%ecx\n"
        "cmpxchg %%rcx, (%%rbx)\n"
        "jz out\n"
        "movq %1, %%rdi\n"
        "callq task_q_blocker\n"
        "jmp trylock\n"
        "out:\n"
        "nop\n"
        :
        : "r" (&(l->key)), "m" (l));
    return;
}



void task_q_unlock(task_lock_t *l)
{
    block_sched();
    if (l->queue != NULL) {
        q_elem *wakeup = queue_pop(l->queue);
        if (wakeup != NULL) {
            unblock_task((Task *) wakeup -> data);
            free(wakeup);
        }
    }
    l -> key = 1;
    unblock_sched();
    return;
}


