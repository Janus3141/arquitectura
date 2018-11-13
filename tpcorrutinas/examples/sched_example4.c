
#include "scheduler.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>
#include <poll.h>

#define check_print(l,s) (write(STDOUT_FILENO, "check: "l"\n", 8+s))


void *f1(void *a)
{
    char thearray[100];
    long long counter = 0;
    while (1) {
        for (char a = 'a'; a <= 'z'; a++) {
            for (char i = 0; i < 100; i++)
                thearray[i] = a;
            if (counter % 1000000 == 0) {
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
            if (counter % 1000000 == 0) {
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
    char buffer[100];
    while (1)
    {
        /* Preparo pfd para hacer poll sobre entrada standard */
        struct pollfd pfd = {.fd = STDIN_FILENO,
                             .events = POLLIN};
        /* Si task_poll retorna algo que no sea mayor que cero es un
           error, asi que se verifica. Tambien se verifica que la
           condicion que hizo retornar a poll sea POLLIN, es decir que
           haya algo para leer (lado derecho de &&) */
        if (task_poll(&pfd, 1) && pfd.revents & POLLIN) {
            ssize_t hm = read(STDIN_FILENO, buffer, 100);
            write(STDOUT_FILENO, buffer, hm);
        }
        else
            write(STDOUT_FILENO, "Poll error detected", 19);
    }
    return 0;
}

