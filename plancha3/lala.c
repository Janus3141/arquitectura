#include <stdio.h>


void avogadro_ieee(short *mantisa) {
    /* Para el exponente, queremos calcular x / 10^23 = 2^x 
        <=>  log 10^23 = log 2^x    (log base 2)
        <=>  23 * (log 10) = x * (log 2) = x
        <=>  23 * (log 10 / log 2) = 23 / log 2 = x  (Cambio de base, log base 10)
        
        Esto da como resultado x = 76.40434618. Luego
            6.02252 x 10^23 = 6.02252 x 2^76.40434618 = 6.02252 x 2^0.40434618 x 2^76
            ~= 7.97073884 x 2^76

        La parte entera de 7.97073884 es 7 en base 10, o 111 en base 2.
        Para normalizarlo se deberá correr la coma 2 lugares a la izquierda, sumando
        además 2 al exponente calculado.
        Para calcular los restantes 50 dígitos que se guardarán para este número como
        double se utiliza el siguiente programa
    */

    double avg = 0.97073884;
    for (int i = 0; i < 50; i++) {
        avg *= 2;
        if (avg > 1) {
            avg -= 1;
            mantisa[i] = 1;
        }
        else
            mantisa[i] = 0;
    }
    return;
}

int main(int argc, char const *argv[])
{
    short man[50];
    avogadro_ieee(man);
    for (int i = 0; i < 50; i++)
        printf("%d", man[i]);
    puts("\n");
    return 0;
}
