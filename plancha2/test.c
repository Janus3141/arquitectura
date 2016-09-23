#include <stdio.h>
#include <string.h>

int findstr(char *str, char *s, long l);

int cmpstr(char *a, char *b, long l);

int fuerzabruta(char *a, char *b, long alen, long blen);

int main(int argc, char *argv[]) {
    int sl = strlen(argv[2]);
    int fl = strlen(argv[1]);
    printf("Resultado: %d\n", fuerzabruta(argv[1], argv[2], fl, sl));
    return 0;
}
