
/* Implementaciones de funciones del scheduler */

// TO DO
/* Comentario sobre codigo */

#include <signal.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <setjmp.h>
#include "scheduler.h"
#include "pqueue.h"
#include "mm.h"

#define debug() (write(STDOUT_FILENO, "debug\n", 6))
#define debug2() (write(STDOUT_FILENO, "sched\n", 6))


/********** Variables globales **********/

// Backup de manejo de señales
struct sigaction old_yield_sig;
struct sigaction old_new_sig;
struct sigaction old_end_sig;

// Setting por defecto de timers
timer_t sched_timer_ID;
struct itimerspec sched_time_setting = {0};
struct itimerspec sched_stop_timer = {0};

// Puntero para conocer tarea en ejecucion
q_elem *qelem_current = NULL;

// Puntero a direccion de stack considerada inicio del sched
void *initial_rsp;

// Puntero a las colas de tareas
pqueue *queues;


/********** Definiciones de funciones **********/

void __error(char *m, size_t n)
{
    write(STDERR_FILENO, m, n);
    _Exit(EXIT_FAILURE);
}


void create_routine(TaskFunc f, void *arg, Task *new)
{
    /* Primero se desarma el timer para ejecutar malloc de forma segura */
    if (timer_settime(sched_timer_ID, 0, &sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
    jmp_buf creator_buf;
    jmp_buf *jb = malloc(sizeof(jmp_buf));
    new->buf = jb;
    new->st = READY;
    new->arg = arg;
    new->fun = f;
    new->mem_position = take_stack();
    if (setjmp(creator_buf) == 0) {
        asm("movq %0, %%rdi\n"
            "movq %1, %%rsi\n"
            "movq %2, %%rsp\n"
            "call _start_routine"
            :
            : "m" (new), "rm" (&creator_buf), "m" (new->mem_position));
    }
    else {    
        union sigval task_pointer = {.sival_ptr = new};
        if (sigqueue(getpid(), SIG_TASK_NEW, task_pointer))
            __error("sigqueue error",14);
    }
    return;
}


void _start_routine(Task *new, jmp_buf *b)
{
    /* Solo para ser llamada por create_routine() */
    if (setjmp(*(new->buf)) == 0) {
        longjmp(*b,1);
    }
    else {
        new -> res = (new->fun)(new->arg);
        new -> st = ZOMBIE;
        stop_routine(new);
    }
}


void stop_routine(Task *r)
{
    /* Si se quiere saber si la funcion termino antes de llamar a stop,
       verificar state */
    if (timer_settime(sched_timer_ID, 0, &sched_stop_timer, NULL))
        __error("Error disarming timer", 21);
    if (r->st != ZOMBIE)
        r->res = NULL;
    free(r->buf);
    release_stack(r);
    FINALIZE(r);
}


void *join_routine(Task *to)
{
    while (to -> st != ZOMBIE) {
        if (timer_settime(sched_timer_ID, 0, &sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
        scheduler(SIG_TASK_YIELD, NULL, NULL);
    }
    return to->res;
}


void block_routine(Task *target)
{
    if (target -> st == READY)
        target -> st = BLOCKED;
}


void unblock_routine(Task *target)
{
    struct itimerspec old;
    if (timer_settime(sched_timer_ID, 0, &sched_stop_timer, &old))
        __error("Error disarming timer", 21);
    target -> st = READY;
    if (target -> queued == 0) {
        union sigval task_pointer = {.sival_ptr = target};
        siginfo_t data = {.si_value = task_pointer};
        scheduler(SIG_TASK_NEW, &data, NULL);
    }
    else {
        old.it_interval = (struct timespec) {0};
        timer_settime(sched_timer_ID, 0, &old, NULL);
    }
}


void scheduler(int signum, siginfo_t *data, void* extra)
{
    static Task *current_task;

    debug2();
    /* Guardar tarea anterior y guardar checkpoint */
    if (signum != SIG_TASK_END && qelem_current != NULL) {
        if (qelem_current -> lvl < QUEUE_NUMBER - 1)
            qelem_current -> lvl ++;
        queue_insert(queues, qelem_current);
        current_task = (Task *) qelem_current -> data;
        current_task -> queued = 1;
        if (current_task -> st == ACTIVE)
            current_task -> st = READY;
        YIELD(current_task);
    }

    /* Encolar nueva tarea en la cola de mayor prioridad */
    if (signum == SIG_TASK_NEW) {
        queue_new_node(queues, data -> si_value.sival_ptr);
        ((Task *) data -> si_value.sival_ptr) -> queued = 1;
    }

    /* Desencolar proxima tarea a ser ejecutada */
    while (1) {
        qelem_current = queue_pop(queues);
        current_task = (Task *) qelem_current -> data;
        current_task -> queued = 0;
        if (current_task -> st == BLOCKED)
            free(qelem_current); // Abstraerlo a pqueue?
        else
            break;
    }

    current_task -> st = ACTIVE;
    sched_time_setting.it_value.tv_nsec = TIME_L(qelem_current -> lvl);
    /* sched_time_setting.it_value.tv_sec = 3;*/
    if (timer_settime(sched_timer_ID, 0, &sched_time_setting, NULL))
        __error("Error setting timer", 19);
    ACTIVATE(current_task);
}


void start_sched(Task *maintask)
{
	/* Setting de timer, señales y handler */
    sigset_t block_these;
    if(sigfillset(&block_these))
        __error("Error in signal config", 22);
    sigdelset(&block_these, SIG_TASK_YIELD);
    sigdelset(&block_these, SIG_TASK_NEW);
    sigdelset(&block_these, SIGTSTP);
    sigdelset(&block_these, SIGKILL);
    struct sigaction sa = {.sa_sigaction = scheduler,
                           .sa_mask = block_these,
                           .sa_flags = SA_SIGINFO | SA_RESTART
                          };
    if (sigaction(SIG_TASK_YIELD, &sa, &old_yield_sig))
        __error("Error setting sigaction", 23);
    if (sigaction(SIG_TASK_NEW, &sa, &old_new_sig))
        __error("Error setting sigaction", 23);
    if (sigaction(SIG_TASK_END, &sa, &old_end_sig))
        __error("Error setting sigaction", 23);
    struct sigevent se = {.sigev_notify = SIGEV_SIGNAL,
                          .sigev_signo = SIG_TASK_YIELD};
    if (timer_create(CLOCK_REALTIME, &se, &sched_timer_ID))
        __error("Error creating timer", 20);

    /* Setting de maintask */
    jmp_buf *jb = malloc(sizeof(jmp_buf));
    maintask -> buf = jb;
    maintask -> st = ACTIVE;
    maintask -> arg = NULL;
    maintask -> fun = NULL;
    maintask -> res = NULL;
    maintask -> queued = 0;
    /* initial_rsp se inicia con el rsp actual */
    asm("movq %%rsp, %0"
    	: "+m" (initial_rsp));
    maintask -> mem_position = take_stack();

    /* Creacion de colas para tareas */
    queues = malloc(sizeof(pqueue));
    *queues = queue_create(QUEUE_NUMBER);

    /* Primer llamado al handler (sched) */
    //YIELD(maintask);
    if (setjmp(*jb) != 0)
        return;
    union sigval task_pointer = {.sival_ptr = maintask};
    // sigqueue(getpid(),SIG_TASK_NEW,task_pointer);
    siginfo_t inf = {.si_value = task_pointer};
    scheduler(SIG_TASK_NEW, &inf, NULL);
}


void sched_blocker(char act) {
    // act = 0 -> block
    // act = 1 -> unblock 
    static struct itimerspec old;
    if (act == 0) {
        if (timer_settime(sched_timer_ID, 0, &sched_stop_timer, &old))
            __error("Error disarming timer",21);
    }
    else if (act == 1) {
        if (timer_settime(sched_timer_ID, 0, &old, NULL))
            __error("Error setting timer",19);
    }
    return;
}      


void destroy_sched(void)
{
    if (timer_settime(sched_timer_ID, 0, &sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
    if (queues != NULL)
        queue_destroy(queues);
    if (qelem_current != NULL)
        free(qelem_current);
    if (sigaction(SIG_TASK_YIELD, &old_yield_sig, NULL))
        __error("Error setting sigaction", 23);
    if (sigaction(SIG_TASK_NEW, &old_new_sig, NULL))
        __error("Error setting sigaction", 23);
    if (sigaction(SIG_TASK_END, &old_end_sig, NULL))
        __error("Error setting sigaction", 23);
    return;
}

