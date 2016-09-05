#include <stdio.h>
#include <math.h>
#include <gmp.h>


typedef struct {
    unsigned short n[16];
} long_number;


void print(long_number n)
{
    int i;
    char str[1024];
    short sign[16], num[16];
    mpz_t n1, n2;
    for (i=0; i<15; i++) {
        num[i] = n.n[i];
        sign[i] = 0;
    }
    num[15] = n.n[15] & 0x7FFF;
    sign[15] = n.n[15] & 0x8000;
    mpz_init(n1);
    mpz_init(n2);
    mpz_import(n1, 16, -1, 2, 0, 0, num);
    mpz_import(n2, 16, -1, 2, 0, 0, sign);
    mpz_neg(n2, n2);
    mpz_add(n2, n1, n2);
    mpz_get_str(str, 10, n2);
    printf("%s\n", str);
    mpz_clear(n1);
    mpz_clear(n2);
}


int is_zero(long_number n) {
    for (int i = 0; i < 16; i++)
        if (n.n[i])
            return 0;
    return 1;
}


int is_one(long_number n) {
    if (n.n[0] != 1)
        return 0;
    for (int i = 1; i < 16; ++i)
        if (n.n[i])
            return 0;
    return 1;
}


int is_even(long_number n) {
    return ((1 & n.n[0]) == 1)? 0 : 1;
}


long_number rshift(long_number n) {
    int lsb;
    n.n[0] = n.n[0] >> 1;
    for (int i = 1; i < 16; i++) {
        lsb = (n.n[i] & 1)? 1 : 0;
        n.n[i-1] = n.n[i-1] | (lsb << (sizeof(short)*8-1));
        n.n[i] = n.n[i] >> 1;
    }
    return n;
}


long_number lshift(long_number n) {
    int msb;
    n.n[15] = n.n[15] << 1;
    for (int i = 14; i >= 0; i--) {
        msb = (n.n[i] & (1 << (sizeof(short)*8-1)))? 1 : 0;
        n.n[i+1] = n.n[i+1] | msb;
        n.n[i] = n.n[i] << 1;
    }
    return n;
}


long_number sum(long_number a, long_number b) {
    // Suma de long_number's bit a bit.
    int carry = 0, mask = 1;
    /* carry y mask se declaran enteros y no shorts
    * para ahorrarse problemas al llegar al ultimo bit
    * de cada short de la estructura. */
    short abit = 0, bbit = 0;
    long_number result;
    for (int i = 0; i < 16; i++)
        result.n[i] = 0;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < sizeof(short)*8; j++) {
            abit = (a.n[i] & mask)? 1 : 0;
            bbit = (b.n[i] & mask)? 1 : 0;
            mask = mask << 1;
            /* Se diferencian los casos:
            *   - Ambos bits son 1
            *   - Solo uno es 1
            *   - Ambos son 0 */
            if (abit & bbit) {
                result.n[i] = result.n[i] | carry;
                carry = mask;
            }
            else if (abit ^ bbit) {
                if (carry == 0)
                    result.n[i] = result.n[i] | (mask >> 1);
                else
                    carry = mask;
            }
            else {
                result.n[i] = result.n[i] | carry;
                carry = 0;
            }
        }
        mask = 1;
        carry = carry? 1 : 0;
    }
    return result;
}


long_number mult(long_number a, long_number b) {
    if (is_zero(b)) {
        long_number zero;
        for (int i = 0; i < 16; i++)
            zero.n[i] = 0;
        return zero;
    }
    else if (is_one(b))
        return a;
    else {
        if (is_even(b))
            return mult(lshift(a),rshift(b));
        else
            return sum(mult(lshift(a),rshift(b)), a);
    }
}


/*
* El programa utiliza las funciones implementadas para
* enteros de 16 unsigned shorts de largo. Se imprimen, en
* salida estandar, dos de estos numeros, y luego su producto.
*/
int main(void) {
    long_number a, b;
    for (int i = 0; i < 16; i++) {
        a.n[i] = 0;
        b.n[i] = 0;
    }
    a.n[0] = 15292;
    b.n[0] = 59292;
    a.n[1] = 12;
    b.n[1] = 3445;

    printf("a: "); print(a);
    printf("b: "); print(b);
    printf("a x b: "); print(mult(a,b));
    return 0;
}
