#include <stdio.h>
#include <string.h>

void cypher(char *word, int size, char key) {
	for(int i = 0; i < size; i++)
		word[i] ^= key;
}

int main(int argc, char *argv[]) {
	if (argc < 3)
		return 1;
	else {
		int sz = strlen(argv[1]);
		cypher(argv[1], sz, *argv[2]);
		printf("Result: %s\n", argv[1]);
		cypher(argv[1], sz, *argv[2]);
		printf("Result: %s\n", argv[1]);
		return 0;
	}
}
