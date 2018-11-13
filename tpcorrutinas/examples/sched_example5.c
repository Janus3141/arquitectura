
#include "scheduler.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#define check_print(l,s) (write(STDOUT_FILENO, "check: "l"\n", 8+s))

int counter = 0;
int counter2 = 0;

task_sp_t mutex = TASK_SP_INIT;


void *f1(void *a)
{
    int my_counter;
    for (int i = 0; i < 10000000; i++) {
        my_counter = counter;
        my_counter++;
        counter = my_counter;
    }
    return NULL;
}


void *f2(void *a)
{
    int my_counter;
    for (int i = 0; i < 10000000; i++) {
        task_sp_lock(&mutex);
        my_counter = counter2;
        my_counter++;
        counter2 = my_counter;
        task_sp_unlock(&mutex);
    }
    return NULL;
}


int main(void)
{
    Task maint, f1t, f2t, f3t, f4t;
    start_sched(&maint);
    /*create_task(f1, NULL, &f1t);
    create_task(f1, NULL, &f2t);
    join_task(&f1t);
    join_task(&f2t);*/
    //printf("Counter is %d\n", counter);
    create_task(f2, NULL, &f3t);
    create_task(f2, NULL, &f4t);
    join_task(&f3t);
    join_task(&f4t);
    destroy_sched();
    check_print("sched destroyed",15);
    printf("Counter2 is %d\n", counter2);
    return 0;
}

