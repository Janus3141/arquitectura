#include <unistd.h>
#include <stdio.h>
#include <sched.h>

#define print(x) (write(1,x,strlen(x)))

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


int main(void)
{
    start_sched();
    task t1, t2, t3;
    create_routine(f1,NULL,&t1);
    create_routine(f2,NULL,&t2);
    create_routine(f3,NULL,&t3);
    f4();
    return;
}
