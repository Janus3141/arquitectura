#include <stdio.h>

void printbin(int n) {
	int mask;
	for (int i = sizeof(int)*8-1; i >= 0; i--) {
		mask = 1 << i;
		printf("%s", (n & mask)? "1":"0");
	}
	putchar('\n');
}

int main(void) {
	printbin(1 << 31);
	printbin((1 << 31) | (1 << 15));
	printbin(-1 & -256);
	printbin(0xaa | (0xaa << 24));
	printbin(5 << 8);
	printbin(-1 & ( ~ (1 << 8)));
	return 0;
}
