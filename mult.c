#include <stdio.h>
#include <stdlib.h>


unsigned mult(unsigned a, unsigned b) {
    if (b == 0)
        return 0;
    else if (b == 1)
        return a;
    else
        return mult((a<<1),b>>1) + mult(a,(b & 1));
}


int main(int argc, char const *argv[])
{
    if (argc < 3)
        return 1;
    else {
        unsigned m = mult(atoi(argv[1]), atoi(argv[2]));
        printf("= %d\n", m);
    }
    return 0;
}
