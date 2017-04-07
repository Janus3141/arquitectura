#include <stdio.h>
#include <signal.h>
#include "guindows2.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>


void error(char *m)
{
    exit(perror(m), 1);
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

    static timer_t timerID;
    timer_create(CLOCK_REALTIME, &se, &timerID);

    static struct itimerspec time_setting = {.it_interval.tv_sec = 0, 
                                            .it_interval.tv_nsec = 0,
                                            .it_value.tv_sec = 0 };

    if (signum == TASK_NEW) {
        task *t = data->si_value.sival_ptr;
        if (queue_size(L0) != 0) {
            queue_insert(L0, t);
            t = (task *) queue_pop(L0);
        }
        time_setting.it_value.tv_nsec = TIME_L0;
        timer_settime(timerID, 0, &time_setting, NULL);
        ACTIVATE(t); // Cambiar definicion de ACTIVATE
    }

    else if (signum == TASK_YIELD) {
        
    }
}


int main(void)
{
    return 0;
}
