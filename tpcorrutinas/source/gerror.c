
/********** Funcion de error **********/

#include <unistd.h>

void __error(char *m, size_t n)
{
    write(STDERR_FILENO, m"\n", n+1);
    _Exit(EXIT_FAILURE);
}

