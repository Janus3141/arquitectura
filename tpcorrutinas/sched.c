#include <stdio.h>
#include <signal.h>
#include "guindows2.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>


void error(char *m)
{
    exit((perror(m), 1));
}


void start_sched(task *t)
{
    timer_t timerID;
    if (!(timer_create(CLOCK_REALTIME, &se, &timerID)))
        error("Error creating timer");
    sched()
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

    static struct itimerspec time_setting = {.it_interval.tv_sec = 0, 
                                            .it_interval.tv_nsec = 0,
                                            .it_value.tv_sec = 0 };

    static task *t;
    t = (task *) data->si_value.sival_ptr;

    static int index = 0;

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
            if (queue_size(t_queues[index]) > 0) {
                if ((t = (task *) queue_pop(t_queues[index]) == NULL))
                    error("Task is NULL");
                break;
            }
        }
    }

    time_setting.it_value.tv_nsec = TIME_L(index);
    if (!(timer_settime(timerID, 0, &time_setting, NULL)))
        error("Error setting timer");
    ACTIVATE(t); // Cambiar definicion de ACTIVATE
}


int main(void)
{
    return 0;
}
