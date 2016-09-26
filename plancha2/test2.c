#include <stdio.h>
#include <string.h>

int findstr(char *str, char *s, long l);

int cmpstr(char *a, char *b, long l);

int fuerzabruta(char *a, char *b, long alen, long blen);

int main(int argc, char *argv[]) {
    printf("Resultado: %d\n", fuerzabruta("ddunacade", "de", 9, 2));
    return 0;
}
