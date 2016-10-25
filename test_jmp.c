#include <stdio.h>
#include <setjmp.h>

int setjmp2(jmp_buf);

void longjmp2(jmp_buf, int value);

int main() {
    jmp_buf mybuf;
    if (setjmp2(mybuf) == 0) {
        printf("In setjmp == 0");
        longjmp2(mybuf,100);
    }
    else
        printf("In setjmp != 0");
    return 0;
}
