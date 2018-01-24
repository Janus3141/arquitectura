
/********** Funcion de error **********/

#include <unistd.h>
#include <stdlib.h>

void __error(char *m, size_t n)
{
    write(STDERR_FILENO, m, n);
    write(STDERR_FILENO, "\n", 1);
    _Exit(EXIT_FAILURE);
}

