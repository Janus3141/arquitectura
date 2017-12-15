
#include "../scheduler.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>

/*
void *f1(void *a)
{
    while (1)
        print("Soy f1");
}


void *f2(void *a)
{
    while (1)
        print("Soy f2");
}


void *f3(void *a)
{
    while (1)
        print("Soy f3");
}
*/


int main(void)
{
    Task maint;
    char arr[1000];
    start_sched(&maint);
    int ind = 0;
    for (int i = 0; i < 1000; i++) {
        for (char j = 'a'; j <= 'z'; j++)
            arr[i] = j;
        if (ind%100 == 0)
            puts("check");
        ind++;
    }   
    /*
    union sigval sv = {.sival_ptr = &maint};
    if (sigqueue(getpid(), SIG_TASK_YIELD, sv) != 0)
        puts("sigqueue error");
    */
    // sleep(1);
    return 0;
}

