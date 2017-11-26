
// TO DO
/* Comentario sobre codigo */

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <queue.h>



/********** Global variables **********/

// Queues for task pointer storing
queue __q0 = queue_init();
queue __q1 = queue_init();
queue __q2 = queue_init();
queue __q3 = queue_init();
queue __q4 = queue_init();
queue _t_queues[] = {__q0, __q1, __q2, __q3, __q4};

// Default timer setting
timer_t _sched_timer_ID;
itimerspec _sched_time_setting = {0};



/********** Function definitions **********/

void __error(char *m, size_t n)
{
	write(STDERR_FILENO, m, n);
    _Exit(EXIT_FAILURE);
}


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


void create_routine(tareaF f, void *arg, task *new)
{
    jmp_buf *ret = malloc(sizeof(jmp_buf));
    if (setjmp(ret) == 0)
        __start_routine(f,arg,new,ret);
    else
        free(ret);
    return;
}


void __start_routine(tareaF f, void *arg, task *new, jmp_buf *b)
{
    /* Solo para ser llamada por create_routine() */
    *new = {0}; /* Inicializo la estructura en 0 para no hacerlo por componente */
    jmp_buf *jb = malloc(sizeof(jmp_buf));
    new->buf = jb;
    new->st = READY;
    union sigval task_pointer = {.sival_ptr = new};
    if (setjmp(*jb) == 0) {
        sigqueue(getpid(), TASK_NEW, task_pointer);
        longjmp(*b,1);
    }
    else {
        new->mem_start = take_stack();
        new->res = f(arg);
        new->st = ZOMBIE;
        FINALIZE(task_pointer);
    }
}


fret stop_routine(task *t)
{
    fret fr;
    if (t->st == ZOMBIE) {
        fr.result = t->res;
        fr.end = 1;
    }
    else {
        FINALIZE(t);
        fr.result = NULL;
        fr.end = 0;
    }
    return fr;
}


void *join_routine(task *from, task *to)
{
    while (to -> st != ZOMBIE) {
        _sched_time_setting = {0};
        if (timer_settime(_sched_timer_ID, 0, &_sched_time_setting, NULL))
            __error("Error disarming timer", 21);
        union sigval task_pointer = {.sival_ptr = from};
        sigqueue(getpid(), TASK_YIELD, task_pointer);
    }
    return to->res;
}


void start_sched(void)
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
    task *maintask = malloc(sizeof(task));
    *maintask = {.buf = jb, st = READY};
    union sigval task_pointer = {.sival_ptr = maintask};
    sigqueue(getpid(), START_SCHED, task_pointer);
    return;
}


void __sched(int signum, siginfo_t *data, void* extra)
{
    static timer_t timerID = (timer_t) *extra;
    static struct itimerspec time_setting = {0};
    static int index = 0;
    static task *t;


    t = (task *) data -> si_value.sival_ptr;

    if (t->st == ACTIVE)
        t -> st = READY;
    YIELD(t);

    if (signum == TASK_NEW) {
        if (queue_size(__q0) > 0) {
            queue_insert(__q0, t);
            t = (task *) queue_pop(__L0);
        }
        index = 0;
    }

    else if (signum == TASK_YIELD) {
        if (t->level < 4)
            t->level++;
        queue_insert(_t_queues[t->level], t);
        for (index = 0; index < 5; index++) {
            while (queue_size(_t_queues[index]) > 0) {
                if ((t = (task *) queue_pop(_t_queues[index]) != NULL))
                    break;
            }
        }
    }

    else if (signum == START_SCHED) {
        t->st = ACTIVE;
        return;
    }

    t->st = ACTIVE;
    _sched_time_setting.it_value.tv_nsec = TIME_L(index);
    if (timer_settime(_sched_timer_ID, 0, &_sched_time_setting, NULL))
        __error("Error setting timer", 19);
    ACTIVATE(t);
}
