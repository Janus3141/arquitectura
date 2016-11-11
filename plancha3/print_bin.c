#include <stdio.h>
#define EXPONENTE_S(f) (*(int *) &(f) >> 23 & 0xff)
#define MANTISA_S(f) (*(int *) &(f) & 0x7fffff)
#define EXPONENTE_D(f) (*(long long *) &(f) >> 52 & 0x7ff)
#define MANTISA_D(f) (*(long long *) &(f) & 0xfffffffffffffLL)

void print_bin(long long n, int digits) {
    long long mask = 1;
    for (int i = digits - 1; i >= 0; i--)
        printf("%d", (mask << i) & n? 1 : 0);
    return;
}


void avogadro_ieee_2(void) {
    /* Aquí se verá cómo se guarda el número de Avogadro con las macros definidas */
    float avg1 = 6.02252e23;
    double avg2 = 6.02252e23;
    printf("avg1 = "); print_bin((long long) EXPONENTE_S(avg1), 8); printf(" | "); print_bin((long long) MANTISA_S(avg1), 23);
    puts("\n");
    printf("avg2 = "); print_bin((long long) EXPONENTE_D(avg2), 11); printf(" | "); print_bin(MANTISA_D(avg2), 52);
    return;
}


int main(int argc, char const *argv[])
{
    double gg = 1.5e2;
    printf("gg = "); print_bin((long long) EXPONENTE_D(gg), 11); printf(" | "); print_bin(MANTISA_D(gg), 52);
    return 0;
}


// 8.327.243
