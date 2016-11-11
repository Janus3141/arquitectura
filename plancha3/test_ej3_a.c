#include <stdio.h> 

int solve(float a, float b, float c, float d, float e, float f, float *x, float *y); 

int main(){
	float x; 
	float y; 
	printf("%i\n", solve(0.5,1,1,0,2,1,&x,&y)); 
	printf("%f\n", x); 
	printf("%f\n", y); 
	return 0; 
}
