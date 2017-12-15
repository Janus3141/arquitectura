
#include "../scheduler.h"
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
    create_routine(f1, NULL, &f1t);
    char thearray[100];
    int counter = 0;
    block_routine(&f1t);
    while (counter < 1000000) {
        for (char a = 'a'; a <= 'z'; a++) {
            for (char i = 0; i < 100; i++)
                thearray[i] = a;
            if (counter % 100000 == 0)
                debug2();
            counter++;
        }
    }
    unblock_routine(&f1t);
    join_routine(&f1t);
    return 0;
}

