
#include "locks2.h"
#include "pqueue.h"
#include "scheduler.h"
#include <malloc.h>
#include <unistd.h>

#define debug(n) (write(STDOUT_FILENO,"debug "n"\n",8))


void task_lock_destroy(task_lock_t *l)
{
    /* Destruye la cola, si fue creada. Se bloquea el
       scheduler para asegurar free */
    block_sched();
    if (l -> queue != NULL)
        queue_destroy(l->queue);
    unblock_sched();
    return;
}


void task_q_blocker(task_lock_t *l)
{
    /* Para ser llamada desde task_q_lock */
    /* Mientras la llave sea 0 se bloquea la tarea, y
       se la inserta en una cola (se crea si es necesario) */
    /* Es necesario bloquear el scheduler para ejecutar malloc
       (queue_new_node) y para probar el valor de key 'atomicamente' */
    while (1) {
        block_sched();
        if (l->key == 1) {
            unblock_sched();
            return;
        }
        if (l -> queue == NULL)
            l -> queue = queue_create(1);
        queue_new_node(l->queue, task_current());
        task_state(BLOCKED);
        task_yield();
    }
}


void task_q_lock(task_lock_t *l)
{
    /* Tratat de conseguir el lock, si no lo
       logra se llama a task_q_blocker */
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
    /* Para soltar el lock pone un 1 en key. Antes se
       asegura de despertar a alguna tarea en la cola, si hay */
    /* Se bloquea el scheduler para probar el valor
       de queue y cambiar el de key 'atomicamente' */
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


