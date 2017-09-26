
// TO DO
/* Comentario sobre codigo */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>


void error(char *m)
{
    exit((perror(m), 1));
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
        start_routine(f,arg,new,ret);
    else
        free(ret);
    return;
}


void start_routine(tareaF f, void *arg, task *new, jmp_buf *b)
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
    while();
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


void start_sched(task *t)
{
    sigset_t block_these;
    if(!(sigfillset(&block_these)))
        error("Error in signal config");
    struct sigaction sa = {.sa_sigaction = sched,
                          .sa_mask = block_these,
                          .sa_flags = SA_SIGINFO
                          };
    if (!(sigaction(TASK_YIELD, &sa, NULL)))
        error("Error setting sigaction");
    if (!(sigaction(TASK_NEW, &sa, NULL)))
        error("Error setting sigaction");
    struct sigevent se = {.sigev_notify = SIGEV_SIGNAL,
                         .sigev_signo = TASK_YIELD};
    if (!(timer_create(CLOCK_REALTIME, &se, &timerID)))
        error("Error creating timer");
    siginfo_t data = {.si_value.sival_ptr = t};
    jmp_buf *jb = malloc(sizeof(jmp_buf));
    task maintask = {.buf = jb, st = READY};
    timer_t timerID;
    YIELD(&maintask);
    sched(TASK_NEW, &data, &timerID);
    return;
}


void sched(int signum, siginfo_t *data, void* extra)
{
    static queue L0 = queue_init();
    static queue L1 = queue_init();
    static queue L2 = queue_init();
    static queue L3 = queue_init();
    static queue L4 = queue_init();
    static queue t_queues[] = {L0,L1,L2,L3,L4};

    static struct sigevent se = {.sigev_signo = PROCESS_YIELD};
    static timer_t timerID = (timer_t) *extra;
    static struct itimerspec time_setting = {0};
    static int index = 0;
    static task *t;


    t = (task *) data->si_value.sival_ptr;

    t->st = READY;
    YIELD(t);

    if (signum == TASK_NEW) {
        if (queue_size(L0) > 0) {
            queue_insert(L0, t);
            t = (task *) queue_pop(L0);
        }
        index = 0;
    }

    else if (signum == TASK_YIELD) {
        if (t->level < 4)
            t->level++;
        queue_insert(t_queues[t->level], t);
        for (index = 0; index < 5; index++) {
            while (queue_size(t_queues[index]) > 0) {
                if ((t = (task *) queue_pop(t_queues[index]) != NULL))
                    break;
            }
        }
    }

    t->st = ACTIVE;
    time_setting.it_value.tv_nsec = TIME_L(index);
    if (!(timer_settime(timerID, 0, &time_setting, NULL)))
        error("Error setting timer");
    ACTIVATE(t);
}
