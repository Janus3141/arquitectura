
#include "src/scheduler.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>

#define check_print(l,s) (write(STDOUT_FILENO, "check: "l"\n", 8+s))


void *f1(void *a)
{
    char thearray[100];
    long long counter = 0;
    while (1) {
        for (char a = 'a'; a <= 'z'; a++) {
            for (char i = 0; i < 100; i++)
                thearray[i] = a;
            if (counter % 100000 == 0) {
                check_print("f1",2);
                counter = 0;
            }
            counter++;
        }
    }
    return NULL;
}


void *f2(void *a)
{
    char thearray[100];
    long long counter = 0;
    write(STDOUT_FILENO, (char *) a, 6);
    while (1) {
        for (char a = 'a'; a <= 'z'; a++) {
            for (char i = 0; i < 100; i++)
                thearray[i] = a;
            if (counter % 100000 == 0) {
                check_print("f2",2);
                counter = 0;
            }
            counter++;
        }
    }
    return NULL;
}


int main(void)
{
    Task maint, f1t, f2t;
    start_sched(&maint);
    char f2arg[] = "f2arg\n";
    create_task(f1, NULL, &f1t);
    create_task(f2, f2arg, &f2t);
    char thearray[100];
    long long counter = 0;
    while (1) {
        for (char a = 'a'; a <= 'z'; a++) {
            for (char i = 0; i < 100; i++)
                thearray[i] = a;
            if (counter % 100000 == 0) {
                check_print("main",4);
                counter = 0;
            }
            counter++;
        }
    }
    return 0;
}

