
/* Implementaciones de funciones del scheduler */

// TO DO
/* Comentario sobre codigo */

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <noqueuesched.h>
#include <setjmp.h>



/********** Variables globales **********/

// Default timer setting
timer_t sched_timer_ID;
itimerspec sched_time_setting = {0};
itimerspec _sched_stop_timer = {0};

// Maintain current task executed
q_elem *_task_current = NULL;


/********** Definiciones de funciones **********/

void __error(char *m, size_t n)
{
    write(STDERR_FILENO, m, n);
    _Exit(EXIT_FAILURE);
}

/*   REDO
void take_stack(void)
{
    static int cuantas = 1;
    static int prof;
    prof = TPILA*cuantas;
    char rsp_sub[20];
    sprintf(rsp_sub, "subq $%d, %rsp", prof);
    asm(rsp_sub);
    cuantas++;
    return;
}
*/

void create_routine(TaskFunc f, void *arg, Task *new)
{
    /* Primero se desarma el timer para ejecutar malloc de forma segura */
    if (timer_settime(sched_timer_ID, 0, &_sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
    jmp_buf *ret = malloc(sizeof(jmp_buf));
    if (setjmp(ret) == 0)
        _start_routine(f,arg,new,ret);
    else
        free(ret);
    return;
}


void _start_routine(TaskFunc f, void *arg, Task *new, jmp_buf *b)
{
    /* Solo para ser llamada por create_routine() */
    *new = {0}; /* Inicializo la estructura en 0 para no hacerlo por componente */
    jmp_buf *jb = malloc(sizeof(jmp_buf));
    new->buf = jb;
    new->st = READY;
    union sigval task_pointer = {.sival_ptr = new};
    if (setjmp(*jb) == 0) {
        sigqueue(getpid(), SIG_TASK_NEW, task_pointer);
        longjmp(*b,1);
    }
    else {
        new->mem_start = take_stack(); // Tomar stack antes para guardar task_pointer
        new->res = f(arg);
        new->st = ZOMBIE;
        stop_routine(new);
    }
}


void stop_routine(Task *r)
{
    /* Si se quiere saber si la funcion termino antes de llamar a stop,
       verificar state */
    if (timer_settime(sched_timer_ID, 0, &_sched_stop_timer, NULL))
        __error("Error disarming timer", 21);
    if (r->st != ZOMBIE)
        r->res = NULL;
    free(r->buf);
    // Liberar memoria
    sigqueue(getpid(), SIG_TASK_END, (union sigval) 0); // Como mandar union nula?
}


void *join_routine(Task *to)
{
    while (to -> st != ZOMBIE) {
        if (timer_settime(sched_timer_ID, 0, &_sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
        sigqueue(getpid(), SIG_TASK_YIELD, (union sigval) 0);
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
    if (timer_settime(sched_timer_ID, 0, &_sched_stop_timer, &old))
        _error("Error disarming timer", 21);
    target -> st = READY;
    if (target -> queued == 0) {
        union sigval task_pointer = {.sival_ptr = target};
        sigqueue(getpid(), SIG_TASK_NEW, task_pointer);
    }
    else {
        old.it_interval = 0;
        timer_settime(sched_timer_ID, 0, &old);
    }
}


void start_sched(Task *maintask)
{
    sigset_t block_these;
    if(sigfillset(&block_these))
        __error("Error in signal config", 22);
    struct sigaction sa = {.sa_sigaction = _sched,
                          .sa_mask = block_these,
                          .sa_flags = SA_SIGINFO
                          };
    if (sigaction(SIG_TASK_YIELD, &sa, NULL))
        __error("Error setting sigaction", 23);
    if (sigaction(SIG_TASK_NEW, &sa, NULL))
        __error("Error setting sigaction", 23);
    struct sigevent se = {.sigev_notify = SIGEV_SIGNAL,
                         .sigev_signo = SIG_TASK_YIELD};
    if (timer_create(CLOCK_REALTIME, &se, &sched_timer_ID))
        __error("Error creating timer", 20);
    jmp_buf *jb = malloc(sizeof(jmp_buf));
    *maintask = {.buf = jb};
    union sigval task_pointer = {.sival_ptr = maintask};
    sigqueue(getpid(),SIG_TASK_NEW,task_pointer);
    return;
}

// Introducir manejo de task con state Blocked
void _sched(int signum, siginfo_t *data, void* extra)
{
    static multi_queue queues = queue_create(QUEUE_NUMBER);
    static Task *current_task;

    /* Guardar tarea anterior y guardar checkpoint */
    if (signum != SIG_TASK_END && _task_current != NULL) {
        if (_task_current -> lvl < QUEUE_NUMBER - 1)
            _task_current -> lvl ++;
        queue_insert(&queues, _task_current)
        current_task = (Task *) _task_current -> data;
        current_task -> queued = 1;
        if (current_task -> st == ACTIVE)
            current_task -> st = READY;
        YIELD(current_task);
    }

    /* Encolar nueva tarea en la cola de mayor prioridad */
    if (signum == SIG_TASK_NEW) {
        queue_new_node(queues, data -> si_value.sival_ptr);
        (data -> si_value.sival_ptr) -> queued = 1;
    }

    /* Desencolar proxima tarea a ser ejecutada */
    do
        _task_current = queue_pop(queues);
    while (((Task *) _task_current -> data) -> st == BLOCKED);
    current_task = (Task *) _task_current -> data;
    current_task -> queued = 0;

    current_task -> st = ACTIVE;
    sched_time_setting.it_value.tv_nsec = TIME_L(_task_current -> lvl);
    if (timer_settime(sched_timer_ID, 0, &sched_time_setting, NULL))
        __error("Error setting timer", 19);
    ACTIVATE(current_task);
}

