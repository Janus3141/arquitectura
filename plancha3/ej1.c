#define EXPONENTE(f) (*(int *) &(f) >> 23 & 0xff)
#define MANTISA(f) (*(int *) &(f) & 0x7fffff)

int main(void) {
	float a = 2.25;
	printf("%d\n", EXPONENTE(a));
	printf("%d\n", MANTISA(a));
	return 0;
}
