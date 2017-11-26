#include <math.h>
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
        Para calcular los restantes 50 dígitos de mantisa que se guardarán para este
        número como double se utiliza este programa.
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

    /* Por lo visto anteriormente, y con lo calculado por esta función, el número
        7.97073884 es, en base 2, 1.1111111000100000100101011100110010110000011100100100
        con doble precisión.
        Mientras tanto, al exponente dado (76) se le deben sumar 2 por la normalización hecha,
        entonces nos quedamos con 78. Para utilizarlo en un:
            - Float: se le debe sumar el sesgo de 127; 78 + 127 = 205 = (11001101) en base 2
            - Double: se le suma sesgo de 1023; 78 + 1023 = 1101 = (10001001101) en base 2

        Por lo tanto, en forma IEEE 754 el número de Avogadro se guardará en memoria como:
        - Float: 0 | 11001101 | 11111110001000001001010 (32 bits)
        - Double: 0 | 10001001101 | 1111111000100000100101011100110010110000011100100100 (64 bits)
    */
}


void avogadro_ieee_2(void) {
    /* Aquí se verá cómo se guarda el número de Avogadro con las macros definidas */
    float avg1 = 6.02252e23;
    double avg2 = 6.02252e23;
    printf("avg1 = "); print_bin((long long) EXPONENTE_S(avg1), 8); printf(" | "); print_bin((long long) MANTISA_S(avg1), 23);
    puts("\n");
    printf("avg2 = "); print_bin((long long) EXPONENTE_D(avg2), 11); printf(" | "); print_bin(MANTISA_D(avg2), 52);
    return;
    /* Al ejecutar esta función se imprime en pantalla:
        avg1 = 11001101 | 11111110001000001001011
        avg2 = 10001001101 | 1111111000100000100101011101100111110000100110010011
    */
}

/*
    En float:
        0 | 11001101 | 11111110001000001001010  - Calculado por nosotros
        0 | 11001101 | 11111110001000001001011  - Extraído con macros

    En double:
        0 | 10001001101 | 1111111000100000100101011100110010110000011100100100  - Calculado por nosotros
        0 | 10001001101 | 1111111000100000100101011101100111110000100110010011  - Extraído con macros

    Se aprecian diferencias en los bits menos significativos de la mantisa, problablemente debido
    a errores de redondeo.
*/
