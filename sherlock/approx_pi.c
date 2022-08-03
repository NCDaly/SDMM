#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <inttypes.h>

#define NUM 100000

double circle(double x){
	return 1 - pow(x,2);
}

int main(){
	static uint32_t count = 0;
	double f_x;

	srand(time(0));
	int i;
	for (i = 0; i < NUM; i++){
		double x = (double) rand() / (double) RAND_MAX;
		double y = (double) rand() / (double) RAND_MAX;
		f_x = circle(x);

		if (pow(y,2) <= f_x){
			count += 1;
		}
	}

	printf("count total: %d\n", count);
	printf("est of pi: %f\n", (double)count/((double)NUM)*4);
	return 0;
}

