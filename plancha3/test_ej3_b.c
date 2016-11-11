#include <stdio.h>

int solve(float, float, float, float, float, float, float *, float *);

int main(void) {
	float a = 3, b = 1, c = 2, d = 3, e = 9, f = 13;
	float x,y;
    printf("x = %f, y = %f", x, y);
	printf("Is there solution? %d\n", solve(a,b,c,d,e,f,&x,&y));
	printf("Solutions: x = %f, y = %f\n", x, y);
	return 0;
}

