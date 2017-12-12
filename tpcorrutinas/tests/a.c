
#include <stdio.h>
#include <setjmp.h>
#include <malloc.h>

struct shitty {
    int num;
    char cha;
    void *more;
};


int main(void)
{
    jmp_buf *the_buf = malloc(sizeof(jmp_buf));
    struct shitty shit = {.num = 3, .cha = 'b', .more = the_buf};
    puts("Say the n: ");
    int gotcha = getchar();
    printf("gotcha = %d\n", gotcha);
    struct shitty *a = &shit;
    a->num = gotcha;
    if (setjmp(*the_buf) == 0) {
        puts("set");
        asm("movq %%rsp, %%r12\n"
            "subq $5000, %%r12\n"
            "movl %1, %%eax\n"
            "movl %%eax, (%%r12)\n"
            "subq $5000, %%rsp\n"
            "movq %%rsp, %0\n"
            : "+m" (a)
            : "m" (gotcha));
        printf("shit.num = %d\n", a->num);
        longjmp(*the_buf,1);
    }
    else
        puts("jump");
    return 0;
}
