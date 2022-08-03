#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <inttypes.h>
#include <shmem.h>

#define NUM 100000

double circle(double x){
	return 1 - pow(x,2);
}

int main(){
	int me, npes;
	shmem_init();
	me = shmem_my_pe();
	npes = shmem_n_pes();

	static uint32_t count = 0;
	double f_x;

	srand(time(0)+me);
	for (int i = 0; i < NUM; i++){
		double x = (double) rand() / (double) RAND_MAX;
		double y = (double) rand() / (double) RAND_MAX;
		f_x = circle(x);

		if (pow(y,2) <= f_x){
			count += 1;
		}
	}

	printf("%d: count %d\n", me, count);
	shmem_barrier_all();

	if (me == 0){
		for (int pe = 1; pe < npes; pe++){
			uint32_t toadd = shmem_int_g(&count, pe);
			printf("to add: %d\n", toadd);
			count += toadd;
		}

		printf("%d: count total: %d\n", me, count);
		printf("%d: ratio: %f\n", me, (double)count/((double)NUM * npes));
		printf("%d: est of pi: %f\n", me, (double)count/((double)NUM * npes)*4);
	}

	shmem_finalize();
	return 0;
}
