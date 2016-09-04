#include <stdio.h>
#include <math.h>
#include <gmp.h>


typedef struct {
   short n[16];
} long_number;

void printbin (int v){
    int s = (sizeof(short) * 8); 
    short mask;
    int i;
    for (i=s-1;i>=0;i--){
        mask = 1<<i;
        printf ("%s", (v & mask) ? "1" : "0");
    }
}

void printl(long_number n) {
    for (int i = 15; i >= 0; i--) {
        printbin(n.n[i]);
        putchar(' ');
    }
    putchar('\n');
}

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

int even(long_number n) {
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
    short carry = 0, mask = 1, abit = 0, bbit = 0;
    long_number result;
    for (int i = 0; i < 16; i++)
        result.n[i] = 0;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < sizeof(short)*8; j++) {
            abit = (a.n[i] & mask)? 1 : 0;
            bbit = (b.n[i] & mask)? 1 : 0;
            mask = mask << 1;
            if (abit & bbit) {
                result.n[i] = result.n[i] | carry;
                carry = mask;
            }
            else if ((abit ^ bbit) && carry == 0)  //MAL, VER XOR CON CARRY 1
                result.n[i] = result.n[i] | (mask >> 1);
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

int main(void) {
    long_number LN, LM;
    for (int i = 0; i < 16; i++) {
        LN.n[i] = 0;
        LM.n[i] = 0;
    }
    LN.n[0] = 15292;
    LM.n[0] = 59292;
    LN.n[1] = 12;
    LM.n[1] = 3445;
    //printf("a: ");
    printl(LN);
    //printf("b: ");
    printl(LM);
    long_number suma = sum(LN, LM);
    //printf("a+b: ");
    printl(suma);
    return 0;
}
