#include <stdio.h>
#include <stdlib.h>

/*
* El programa debe recibir un entero como argumento al llamarlo.
* Este entero se usa como llave para cifrar, caracter a caracter,
* texto que se recibe de la entrada estandar, e imprime, tambien
* por caracter, en la salida estandar.
*/

int main(int argc, char *argv[]) {
	if (argc < 2)
		return 1;
	else {
		int c = getchar();
		int key = atoi(argv[1]);
		while (c != EOF) {
			putchar(c^key);
			c = getchar();
		}
		putchar('\n');
		return 0;
	}
}
