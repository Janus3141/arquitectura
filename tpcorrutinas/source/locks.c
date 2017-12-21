
#include "scheduler.h"
#include "locks.h"
#include <malloc.h>
#include "gerror.h"


task_sp_arr_t *task_sp_arr = NULL;
int task_sp_arr_size = 0;

task_sp_t task_sp_manager(char act, task_sp_t n)
{
    static int next_free_spot = 0;
    
    /* Inicializa un nuevo sp */
    if (act == TASK_SP_NEW) {
        block_sched();
        if (next_free_spot >= task_sp_arr_size) {
            int i = task_sp_arr_size;
            task_sp_arr_size += TASK_SP_SIZE_LEAP;
            task_sp_arr = realloc(task_sp_arr,sizeof(task_sp_arr_t)*task_sp_arr_size + 1);
            for (; i < task_sp_arr_size; i++)
                (task_sp_arr[i]).state = TASK_SP_UNINIT;
        }
        task_sp_t result = next_free_spot;
        (task_sp_arr[result]).state = TASK_SP_INIT;
        (task_sp_arr[result]).value = TASK_SP_UNLOCK;
        for (int i = 0; i <= task_sp_arr_size; i++) {
            if ((task_sp_arr[i]).state == TASK_SP_UNINIT)
                next_free_spot = i;
        }
        unblock_sched();
        return result;
    }

    /* Pone al sp dado en estado no iniciado */
    else if (act == TASK_SP_DEL) {
        if (n >= task_sp_arr_size)
            __error("task sp bad argument",23);
        (task_sp_arr[n]).state = TASK_SP_UNINIT;
        if (n < next_free_spot)
            next_free_spot = n;
        return TASK_SP_OK;
    }

    /* Libera la memoria reservada. Si se utiliza algun sp
       despues de este punto probablemente se reciba un segfault */
    else if (act == TASK_SP_DESTROY) {
        free(task_sp_arr);
        return TASK_SP_OK;
    }
}


int task_sp_lock(task_sp_t n)
{
    int own_key = TASK_SP_LOCK;
    while (1) {
        asm("movq %1, %%r12\n"
            "xchg %%r12, %0\n"
            "movq %%r12, %1"
            : "=m" ((task_sp_arr[n]).value), "=m" (own_key));
        if ((task_sp_arr[n]).state != TASK_SP_INIT)
            return TASK_SP_FAIL;
        else if (own_key == TASK_SP_LOCK)
            task_yield();
        else if (own_key == TASK_SP_UNLOCK)
            break;
    }
    return TASK_SP_OK;
}


void task_sp_unlock(task_sp_t n)
{
    (task_sp_arr[n]).value = TASK_SP_UNLOCK;
    return;
}


