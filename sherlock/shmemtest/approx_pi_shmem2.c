#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <inttypes.h>
#include <shmem.h>

#define NUM 100000
#define NUM2 1000

double circle(double x){
	return 1 - pow(x,2);
}

int main(){
	int me, npes;

	long pWrk_odd[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
	long pWrk_even[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
	long pSync_odd[SHMEM_REDUCE_SYNC_SIZE];
	long pSync_even[SHMEM_REDUCE_SYNC_SIZE];
	for (int i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i++){
		pSync_odd[i] = SHMEM_SYNC_VALUE;
		pSync_even[i] = SHMEM_SYNC_VALUE;
	}

	shmem_init();
	me = shmem_my_pe();
	npes = shmem_n_pes();

	static long count = 0;
	double f_x;

	if (me % 2 != 0){
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
	}

	else {
		srand(time(0)+me);
		for (int i = 0; i < NUM2; i++){
 			double x = (double) rand() / (double) RAND_MAX;
                        double y = (double) rand() / (double) RAND_MAX;
                        f_x = circle(x);

			if (pow(y,2) <= f_x){
				count += 1;
			}
		}

		printf("%d: count %d\n", me, count);
	}

	static long total_odd = 0;
	shmem_long_sum_to_all(&total_odd, &count, 1, 1, 1, (npes/2), pWrk_odd, pSync_odd);

	static long total_even = 0;
	shmem_long_sum_to_all(&total_even, &count, 1, 0, 1, (npes/2), pWrk_even, pSync_even);

	if (me == 0){
		printf("%d: even count total: %d\n", me, total_even);
		printf("%d: even ratio: %f\n", me, (double)total_even/((double)NUM2 * (npes/2)));
		printf("%d: even est of pi: %f\n", me, (double)total_even/((double)NUM2 * (npes/2))*4);
	}
	else if (me == 1){
		printf("%d: odd count total: %d\n", me, total_odd);
		printf("%d: odd ratio: %f\n", me, (double)total_odd/((double)NUM * (npes/2)));
		printf("%d: odd est of pi: %f\n", me, (double)total_odd/((double)NUM * (npes/2))*4);
        }


	shmem_finalize();
	return 0;
}
