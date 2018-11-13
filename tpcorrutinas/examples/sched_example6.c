
#include "scheduler.h"
#include "shmem.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <malloc.h>

#define check_print(l,s) (write(STDOUT_FILENO, "check: "l"\n", 8+s))


void *f1(void *arg)
{
    /* Escribir mensaje */
    shmem_t comm = shmem_new('a');
    if (shmem_write(&comm, "Mensaje de f1\n", 14) != 14)
        check_print("Problema de escritura en f1",27);

    /* Leer mensaje de memoria */
    char buffer[30];
    int ans = shmem_block_read(&comm, buffer, 30);
    if (ans > 0) {
        check_print("Este es f1",10);
        write(STDOUT_FILENO, buffer, ans);
    }
    else
        check_print("Problema de lectura en f1",25);

    return NULL;
}


void *f2(void *arg)
{
    /* Leer mensaje de memoria */
    shmem_t comm = shmem_new('a');
    char buffer[30];
    int ans = shmem_block_read(&comm, buffer, 30);
    if (ans <= 0)
        check_print("Problema de lectura en f2",25);
    else {
        check_print("Este es f2",10);
        write(STDOUT_FILENO, buffer, ans);
    }

    /* Hacer algo de trabajo */
    char thearray[100];
    int counter = 0;
    while (counter < 1000000) {
        for (char a = 'a'; a <= 'z'; a++) {
            for (char i = 0; i < 100; i++)
                thearray[i] = a;
            if (counter % 100000 == 0)
                check_print("f2 trabajando",13);
            counter++;
        }
    }

    /* Enviar mensaje */
    if (shmem_write(&comm, "Mensaje de f2\n", 14) != 14)
        check_print("Problema de escritura en f2",27);

    return NULL;
}


int main(void)
{
    Task maint, f1t, f2t;
    start_sched(&maint);
    create_task(f1, NULL, &f1t);
    create_task(f2, NULL, &f2t);
    join_task(&f1t);
    join_task(&f2t);
    destroy_sched();
    return 0;
}

