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

	long pWrk[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
	long pSync[SHMEM_REDUCE_SYNC_SIZE];
	for (int i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i++){
		pSync[i] = SHMEM_SYNC_VALUE;
	}

	shmem_init();
	me = shmem_my_pe();
	npes = shmem_n_pes();

	static long count = 0;
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

	static long total = 0;

	shmem_long_sum_to_all(&total, &count, 1, 0, 0, npes, pWrk, pSync);   

	if (me == 0){
		printf("%d: count total: %d\n", me, total);
		printf("%d: ratio: %f\n", me, (double)total/((double)NUM * npes));
		printf("%d: est of pi: %f\n", me, (double)total/((double)NUM * npes)*4);
	}

	shmem_finalize();
	return 0;
}
