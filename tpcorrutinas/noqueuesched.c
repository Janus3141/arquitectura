
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
timer_t _sched_timer_ID;
itimerspec _sched_time_setting = {0};
itimerspec _sched_stop_timer = {0};

// Maintain current task executed
Task *_current_task;


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
    if (timer_settime(_sched_timer_ID, 0, &_sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
    jmp_buf *ret = malloc(sizeof(jmp_buf));
    if (setjmp(ret) == 0)
        __start_routine(f,arg,new,ret);
    else
        free(ret);
    return;
}


void __start_routine(TaskFunc f, void *arg, Task *new, jmp_buf *b)
{
    /* Solo para ser llamada por create_routine() */
    *new = {0}; /* Inicializo la estructura en 0 para no hacerlo por componente */
    jmp_buf *jb = malloc(sizeof(jmp_buf));
    new->buf = jb;
    new->st = READY;
    union sigval task_pointer = {.sival_ptr = new};
    if (setjmp(*jb) == 0) {
        sigqueue(getpid(), _TASK_NEW, task_pointer);
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
    if (timer_settime(_sched_timer_ID, 0, &_sched_stop_timer, NULL))
        __error("Error disarming timer", 21);
    if (r->st != ZOMBIE)
        r->res = NULL;
    free(r->buf);
    // Liberar memoria
    sigqueue(getpid(), _TASK_END, union sigval {0}); // Como mandar union nula?
}


void *join_routine(Task *to)
{
    while (to -> st != ZOMBIE) {
        if (timer_settime(_sched_timer_ID, 0, &_sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
        union sigval task_pointer = {.sival_ptr = _current_task};
        sigqueue(getpid(), _TASK_YIELD, task_pointer);
    }
    return to->res;
}


void block_routine(Task *target)
{

}


void unblock_routine(Task *target)
{

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
    if (sigaction(_TASK_YIELD, &sa, NULL))
        __error("Error setting sigaction", 23);
    if (sigaction(_TASK_NEW, &sa, NULL))
        __error("Error setting sigaction", 23);
    struct sigevent se = {.sigev_notify = SIGEV_SIGNAL,
                         .sigev_signo = _TASK_YIELD};
    if (timer_create(CLOCK_REALTIME, &se, &_sched_timer_ID))
        __error("Error creating timer", 20);
    jmp_buf *jb = malloc(sizeof(jmp_buf));
    *maintask = {.buf = jb};
    union sigval task_pointer = {.sival_ptr = maintask};
    _current_task = maintask;
    sigqueue(getpid(),_TASK_NEW,task_pointer);
    return;
}

// Introducir manejo de task con state Blocked
void _sched(int signum, siginfo_t *data, void* extra)
{
    static int index = 0;

    static Task *back[];
    static Task *front[];


    if (signum != _TASK_END && signum != _START_SCHED) {
        /* Guardar tarea anterior */
        if (_current_task -> level < 4)
            _current_task -> level++;

        if (back[_current_task -> level] != NULL) {
            _next(back[_current_task -> level]) = _current_task;
            _prev(_current_task) = back[_current_task -> level];
            _next(_current_task) = NULL;
            back[_current_task -> level] = _current_task;
        }
        else {
            back[_current_task -> level] = _current_task;
            front[_current_task -> level] = _current_task;
            _next(_current_task) = NULL;
            _prev(_current_task) = NULL;
        }

        if (_current_task -> st == ACTIVE)
            _current_task -> st = READY;
        YIELD(_current_task);
    }

    if (signum == _TASK_BLOCK) {
        _current_task = (Task *) data -> si_value.sival_ptr;
        if (_prev(_current_task) != NULL)
            _next(_prev(_current_task)) = _next(_current_task);
        else if (front(_current_task -> level) == _current_task)
            front(_current_task) -> level = _next(_current_task);
        _prev(_current_task) = NULL;
        _next(_current_task) = NULL;
    }

    else if (signum == _TASK_UNBLOCK) {
        _current_task = (Task *) data -> si_value.sival_ptr;
        _current_task -> state = READY;
    }

    /* Desencolar tarea lista para ejecutar */
    if (signum == _TASK_NEW || signum == _TASK_UNBLOCK) {
        _current_task = (Task *) data -> si_value.sival_ptr;
        _current_task -> level = 0;
        if (back[0] != NULL) {
                /* Si la cola no esta vacia, encolo la nueva tarea,
                   si esta vacia sigo con la nueva tarea como current */
                _next(back[0]) = _current_task;
                _prev(_current_task) = back[0];
                _next(_current_task) = NULL;
                back[0] = _current_task;
                _current_task = front[0];
                front[0] = _next(_current_task);
                _prev(front[0]) = NULL;
        }
        index = 0;
    }

    else if (signum == _TASK_YIELD || signum == _TASK_END || signum == _TASK_BLOCK) {
        for (index = 0; index < 5; index++) {
            if (front[index] != NULL) {
                _current_task = front[index];
                front[index] = _next(_current_task);
                if (front[index] == NULL && back[index] != NULL)
                    /* Solo ocurre si habia un solo elemento */
                    back[index] = NULL;
                else
                    _prev(front[index]) = NULL;
                break;
            }
        }
    }

    _current_task->st = ACTIVE;
    _sched_time_setting.it_value.tv_nsec = TIME_L(index);
    if (timer_settime(_sched_timer_ID, 0, &_sched_time_setting, NULL))
        __error("Error setting timer", 19);
    ACTIVATE(_current_task);
}
