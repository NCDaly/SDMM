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

	static long count[2] = {0,0};
	double f_x;
	//static long num_rounds = 0;

	srand(time(0)+me);
	for (int i = 0; i < NUM; i++){
		double x = (double) rand() / (double) RAND_MAX;
		double y = (double) rand() / (double) RAND_MAX;
		//printf("x: %f y:%f\n", x, y);
		f_x = circle(x);

		//printf("f_x: %f\n", f_x);
		if (pow(y,2) <= f_x){
			count[0] += 1;
		}

		double n = 0;
		int i = 1;
		long rounds = 0;
		while (i == 1){
			rounds += 1;
			n += (double) rand() / (double) RAND_MAX;
			//printf("r:%d n:%f\n", rounds, n);

			if (n > 1) {
				count[1] += rounds;
				i = 0;
			}
		}
	}

	printf("%d: count %d\n", me, count[0]);
	printf("%d: num_rounds %d\n", me, count[1]);

	static long total[2] = {0,0};

	shmem_long_sum_to_all(&total, &count, 2, 0, 0, npes, pWrk, pSync);   

	if (me == 0){
		printf("%d: e count total: %d\n", me, total[1]);
		printf("%d: est of e:  %f\n", me, (double)total[1]/((double)NUM * npes));

		printf("%d: pi count total: %d\n", me, total[0]);
		printf("%d: pi ratio: %f\n", me, (double)total[0]/((double)NUM * npes));
		printf("%d: est of pi: %f\n", me, (double)total[0]/((double)NUM * npes)*4);

	}

	shmem_finalize();
	return 0;
}
