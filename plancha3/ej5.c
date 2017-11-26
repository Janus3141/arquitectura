#include <time.h>
#include <stdio.h>

void sum(float *, float *, int);
void sum_sse(float *, float *, int);

int main(void) {
	struct timespec start, end;
	float arr[100000];
	double secs;
	if (clock_gettime(CLOCK_REALTIME, &start) == 0) {
		for (int i = 1000; i <= 100000; i++)
			sum(arr, arr, i);
	}
	else
		return 1;

	if (clock_gettime(CLOCK_REALTIME, &end) == 0) {
		secs = end.tv_sec - start.tv_sec;
		printf("Tiempo sum: %lf\n", secs);
	}
	else
		return 1;

	if (clock_gettime(CLOCK_REALTIME, &start) == 0) {
		for (int i = 1000; i <= 100000; i++)
			sum_sse(arr, arr, i);
	}
	else
		return 1;

	if (clock_gettime(CLOCK_REALTIME, &end) == 0) {
		secs = end.tv_sec - start.tv_sec;
		printf("Tiempo sum_sse: %lf\n", secs);
	}
	else
		return 1;

	return 0;
}
