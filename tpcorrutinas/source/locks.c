
#include "scheduler.h"
#include "locks.h"
#include <malloc.h>
#include "gerror.h"


task_mutex_arr_t *task_mutex_arr = NULL;
int task_mutex_arr_size = 0;

task_mutex_t task_mutex_manager(char act, task_mutex_t n)
{
    static int next_free_spot = 0;
    
    /* Inicializa un nuevo mutex */
    if (act == TASK_MUTEX_NEW) {
        block_sched();
        if (next_free_spot >= task_mutex_arr_size) {
            int i = task_mutex_arr_size;
            task_mutex_arr_size += TASK_MUTEX_SIZE_LEAP;
            task_mutex_arr = realloc(task_mutex_arr,sizeof(task_mutex_arr_t)*task_mutex_arr_size + 1);
            for (; i < task_mutex_arr_size; i++)
                (task_mutex_arr[i]).state = TASK_MUTEX_UNINIT;
        }
        task_mutex_t result = next_free_spot;
        (task_mutex_arr[result]).state = TASK_MUTEX_INIT;
        (task_mutex_arr[result]).value = TASK_MUTEX_UNLOCK;
        for (int i = 0; i <= task_mutex_arr_size; i++) {
            if ((task_mutex_arr[i]).state == TASK_MUTEX_UNINIT)
                next_free_spot = i;
        }
        unblock_sched();
        return result;
    }

    /* Pone al mutex dado en estado no iniciado */
    else if (act == TASK_MUTEX_DEL) {
        if (n >= task_mutex_arr_size)
            __error("task mutex bad argument",23);
        (task_mutex_arr[n]).state = TASK_MUTEX_UNINIT;
        if (n < next_free_spot)
            next_free_spot = n;
        return TASK_MUTEX_OK;
    }

    /* Libera la memoria reservada. Si se utiliza algun mutex
       despues de este punto probablemente se reciba un segfault */
    else if (act == TASK_MUTEX_DESTROY) {
        free(task_mutex_arr);
        return TASK_MUTEX_OK;
    }
}


int task_mutex_lock(task_mutex_t n)
{
    int own_key = TASK_MUTEX_LOCK;
    while (1) {
        asm("movq $0, %%r12\n"
            "xchg %%r12, %0\n"
            "movq %%r12, %1"
            : "=m" ((task_mutex_arr[n]).value), "=m" (own_key));
        if ((task_mutex_arr[n]).state != TASK_MUTEX_INIT)
            return TASK_MUTEX_FAIL;
        else if (own_key == TASK_MUTEX_LOCK)
            task_yield();
        else if (own_key == TASK_MUTEX_UNLOCK)
            break;
    }
    return TASK_MUTEX_OK;
}


void task_mutex_unlock(task_mutex_t n)
{
    (task_mutex_arr[n]).value = TASK_MUTEX_UNLOCK;
    return;
}


