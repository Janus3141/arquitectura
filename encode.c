#include <stdio.h>
#include <stdlib.h>

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
		return 0;
	}
}
