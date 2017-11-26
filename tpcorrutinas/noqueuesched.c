
/* Scheduler functions implementations */

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



/********** Global variables **********/

// Default timer setting
timer_t _sched_timer_ID;
itimerspec _sched_time_setting = {0};
itimerspec _sched_stop_timer = {0};

// Maintain current task executed
Task *_t;


/********** Function definitions **********/

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
        if (timer_settime(_sched_timer_ID, 0, &_sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
        sigqueue(getpid(), TASK_NEW, task_pointer);
        longjmp(*b,1);
    }
    else {
        new->mem_start = take_stack(); // Tomar stack antes para guardar task_pointer
        new->res = f(arg);
        new->st = ZOMBIE;
        _finalize_routine(new);
    }
}


Fret stop_routine(Task *tk)
{
    Fret fr;
    if (tk->st == ZOMBIE) {
        fr.result = tk->res;
        fr.end = 1;
    }
    else {
        _finalize_routine(tk);
        fr.result = NULL;
        fr.end = 0;
    }
    return fr;
}


void _finalize_routine(Task *r)
{
    if (timer_settime(_sched_timer_ID, 0, &_sched_stop_timer, NULL))
        __error("Error disarming timer", 21);
    free(r->buf);
    union sigval task_pointer = {.sival_ptr = r};
    sigqueue(getpid(), TASK_END, task_pointer);
}


void *join_routine(Task *to)
{
    while (to -> st != ZOMBIE) {
        if (timer_settime(_sched_timer_ID, 0, &_sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
        union sigval task_pointer = {.sival_ptr = _t};
        sigqueue(getpid(), TASK_YIELD, task_pointer);
    }
    return to->res;
}

// Cambiar estas dos para hacerlas mas especificas
void block_routine(Task *target)
{
    if (timer_settime(_sched_timer_ID, 0, &_sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
    target -> st = BLOCKED;
    while (target -> st == BLOCKED) {
        if (timer_settime(_sched_timer_ID, 0, &_sched_stop_timer, NULL))
            __error("Error disarming timer", 21);
        union sigval task_pointer = {.sival_ptr = _t};
        sigqueue(getpid(), TASK_YIELD, task_pointer);
    }
}


void unblock_routine(Task *target)
{
    target -> st = READY;
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
    if (sigaction(TASK_YIELD, &sa, NULL))
        __error("Error setting sigaction", 23);
    if (sigaction(TASK_NEW, &sa, NULL))
        __error("Error setting sigaction", 23);
    struct sigevent se = {.sigev_notify = SIGEV_SIGNAL,
                         .sigev_signo = TASK_YIELD};
    if (timer_create(CLOCK_REALTIME, &se, &_sched_timer_ID))
        __error("Error creating timer", 20);
    jmp_buf *jb = malloc(sizeof(jmp_buf));
    *maintask = {.buf = jb, st = ACTIVE};
    union sigval task_pointer = {.sival_ptr = maintask};
    _t = maintask;
    _sched_time_setting.it_value.tv_nsec = TIME_L(0);
    if (timer_settime(_sched_timer_ID, 0, &_sched_time_setting, NULL))
        __error("Error setting timer", 19);
    return;
}

// Introducir manejo de task con state Blocked
void __sched(int signum, siginfo_t *data, void* extra)
{
    static int index = 0;

    static Task *l0back = _t, *l0front = _t;
    static Task *l1back, *l1front;
    static Task *l2back, *l2front;
    static Task *l3back, *l3front;
    static Task *l4back, *l4front;
    static Task *back[] = {l0back, l1back, l2back, l3back, l4back};
    static Task *front[] = {l0front, l1front, l2front, l3front, l4front};

    if (signum != TASK_END) {
        /* Guardar tarea anterior */
        if (_t -> level < 4)
            _t -> level++;

        if (back[_t->level] != NULL)
            _next(back[_t->level]) = _t;
        else {
            back[_t->level] = _t;
            front[_t->level] = _t;
        }

        if (_t->st == ACTIVE)
                _t -> st = READY;
        YIELD(_t);
    }

    /* Desencolar tarea lista para ejecutar */
    if (signum == TASK_NEW) {
        _t = (Task *) data -> si_value.sival_ptr;
        if (l0back != NULL) {
            _next(l0back) = _t;
            l0back = _t;
            _t = l0front;
            l0front = _next(_t);
        }
        index = 0;
    }

    else if (signum == TASK_YIELD) {
        for (index = 0; index < 5; index++) {
            if (front[index] != NULL) {
                _t = front[index];
                front[index] = _next(_t);
                if (front[index] == NULL && back[index] != NULL)
                    /* Solo ocurre si habia un solo elemento */
                    back[index] = NULL;
                break;
            }
        }
    }

    _t->st = ACTIVE;
    _sched_time_setting.it_value.tv_nsec = TIME_L(index);
    if (timer_settime(_sched_timer_ID, 0, &_sched_time_setting, NULL))
        __error("Error setting timer", 19);
    ACTIVATE(_t);
}


