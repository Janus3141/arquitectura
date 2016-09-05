#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b, int *c) {
	*b ^= *c;
	*c ^= *b;
	*b ^= *c;
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

/*
* Al llamar al programa se piden tres caracteres a, b y c,
* se llama a la funcion swap con estos caracteres,
* y finalmente se imprime el valor actual de las variables
* donde se guardaron los caracteres.
*/
int main(void) {
	int _a, _b, _c;
	int *a = &_a, *b = &_b, *c = &_c;
	printf("a: "); *a = getchar();
	getchar(); //Tira el caracter \n
	printf("b: "); *b = getchar();
	getchar(); //Tira el caracter \n
	printf("c: "); *c = getchar();
	swap(a,b,c);
	putchar('\n');
	printf("a: %c\n", *a);
	printf("b: %c\n", *b);
	printf("c: %c\n", *c);
	return 0;
}
