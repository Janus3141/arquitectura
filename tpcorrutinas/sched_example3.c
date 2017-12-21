
#include "source/scheduler.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>

#define check_print(l,s) (write(STDOUT_FILENO, "check: "l"\n", 8+s))


void *f1(void *a)
{
    char thearray[100];
    int counter = 0;
    while (1) {
        for (char a = 'a'; a <= 'z'; a++) {
            for (char i = 0; i < 100; i++)
                thearray[i] = a;
            if (counter % 100000 == 0)
                check_print("f1",2);
            counter++;
        }
    }
    char *result = malloc((size_t) 10);
    result = "result\n";
    return result;
}


void *f2(void *a)
{
    char thearray[100];
    int counter = 0;
    write(STDOUT_FILENO, (char *) a, 6);
    while (1) {
        for (char a = 'a'; a <= 'z'; a++) {
            for (char i = 0; i < 100; i++)
                thearray[i] = a;
            if (counter % 100000 == 0)
                check_print("f2",2);
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
    // block_sched();
    char thearray[100];
    int counter = 0;
    while (counter < 1000000) {
        for (char a = 'a'; a <= 'z'; a++) {
            for (char i = 0; i < 100; i++)
                thearray[i] = a;
            if (counter % 100000 == 0)
                check_print("main",4);
            counter++;
        }
    }
    void *f1res = join_task(&f1t);
    write(STDOUT_FILENO, (char *) f1res, 7);
    // free(f1res); // Termina en segfault !!
    join_task(&f2t);
    destroy_sched();
    check_print("sched destroyed",15);
    return 0;
}

