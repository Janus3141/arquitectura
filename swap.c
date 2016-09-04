#include <stdio.h>

void swap(int *a, int *b, int *c) {
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
	*b ^= *c;
	*c ^= *b;
	*b ^= *c;
}

int main(void) {
	int uno = 1, dos = 2, tres = 3;
	int *unoP = &uno, *dosP = &dos, *tresP = &tres;
	swap(unoP, dosP, tresP);
	printf("uno = %d, dos = %d, tres = %d\n", uno, dos, tres);
	return 0;
}
