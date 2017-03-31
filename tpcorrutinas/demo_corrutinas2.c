#include <stdio.h>
#include <signal.h>
#include "guindows2.h"
#include <string.h>
#include <unistd.h>
#include <signal.h>


void sched(int signum, siginfo_t *data, void* extra)
{
    static task *L1_top, *L2_top, *L3_top, *L4_top;
    static task *L1_bottom, *L2_bottom, *L3_bottom, *L4_bottom;

    struct sigevent se;
    se.sigev_signo = PROCESS_YIELD;

    timer_t timerID;
    timer_create(CLOCK_REALTIME, &se, &timerID);

    struct itimerspec time_setting;
    time_setting.it_interval.tv_sec = 0;
    time_setting.it_interval.tv_nsec = 0;
    time_setting.it_value.tv_sec = 0;

    if (signum == TASK_NEW) {
        task *new_task = data->si_value.sival_ptr;
        if (L1_bottom != NULL)
            L1_bottom.next = new_task;
        else
            L1_top = new_task;
        L1_bottom = new_task;
        time_setting.it_value.tv_nsec = TIME_L1;
        timer_settime(timerID, 0, &time_setting, NULL);
        ACTIVATE(L1_top.buf);
    }
    else if (signum == TASK_YIELD) {
    }
}


int main(void)
{
	return 0;
}
