
// TO DO
/* Comentario sobre codigo */

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


void error(char *m)
{
    exit((perror(m), 1));
}


void create_routine(void *(*f)(void *), void *arg, task *new)
{
    jmp_buf ret;
    if (setjmp(ret) == 0)
        start_routine(f,arg,new,ret);
    else
        return;
}


void start_routine(void *(*f)(void *), void *arg, task *new, jmp_buf b)
{
    /* Solo para ser llamada por create_routine() */
    *new = {0}; /* Inicializo la estructura en 0 para no hacerlo por componente */
    jmp_buf *jb = malloc(sizeof(jmp_buf));
    new->buf = jb;
    new->st = READY;
    union sigval task_pointer = {.sival_ptr = new};
    if (setjmp(*jb) == 0) {
        sigqueue(getpid(), TASK_NEW, task_pointer);
        longjmp(b,1);
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


void kill_routine(task *t)
{
    free_stack(t->mem_start);
    free(t->buf);
}


void start_sched(task *t)
{
    // Setear manejo de señales y memoria
    timer_t timerID;
    if (!(timer_create(CLOCK_REALTIME, &se, &timerID)))
        error("Error creating timer");
    siginfo_t data = {.si_value.sival_ptr = t};
    sched(TASK_NEW, &data, &timerID);
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

    if (signum == TASK_KILL) {
        
    }

    t->st = READY;
    YIELD(t);

    else if (signum == TASK_NEW) {
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
            if (queue_size(t_queues[index]) > 0) {
                if ((t = (task *) queue_pop(t_queues[index]) == NULL))
                    error("Task is NULL");
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
