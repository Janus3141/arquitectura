
#include "src/scheduler.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>

#define debug() (write(STDOUT_FILENO, "check: f1\n", 10))
#define debug2() (write(STDOUT_FILENO, "check: main\n", 12))


void *f1(void *a)
{
    char thearray[100];
    int counter = 0;
    while (counter < 1000000) {
        for (char a = 'a'; a <= 'z'; a++) {
            for (char i = 0; i < 100; i++)
                thearray[i] = a;
            if (counter % 100000 == 0)
                debug();
            counter++;
        }
    }
    return NULL;
}


int main(void)
{
    Task maint, f1t;
    start_sched(&maint);
    create_task(f1, NULL, &f1t);
    char thearray[100];
    int counter = 0;
    block_task(&f1t);
    while (counter < 1000000) {
        for (char a = 'a'; a <= 'z'; a++) {
            for (char i = 0; i < 100; i++)
                thearray[i] = a;
            if (counter % 100000 == 0)
                debug2();
            counter++;
        }
    }
    unblock_task(&f1t);
    join_task(&f1t);
    if (f1t.st == ZOMBIE)
        puts("Ok, is zombie");
    else
        puts("Oh no");
    destroy_sched();
    return 0;
}

