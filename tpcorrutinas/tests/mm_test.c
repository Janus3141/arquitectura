
#include <stdio.h>
#include "../mm.c"


void *initial_rsp = NULL;

int main(void)
{
    void *received_dir;
    asm("movq %%rsp, %0"
        : "+m" (initial_rsp));
    printf("rsp = %p\n", initial_rsp);
    for (int i = 0; i < 6; i++) {
        received_dir = take_stack();
        printf("%d>\t received %p\n", i, received_dir);
    }
    void *sixth = take_stack();
    printf("%d>\t received %p\n", 6, sixth);
    for (int i = 7; i < 16; i++) {
        received_dir = take_stack();
        printf("%d>\t received %p\n", i, received_dir);
    }
    memory_manager(sixth,0);
    void *sixteenth = take_stack();
    printf("%d>\t received %p\n", 16, sixteenth);
    printf("%d>\t received %p\n", 6, sixth);
    return 0;
}

