#include <stdio.h>
#include <stdlib.h>
#include <shmem.h>

int main(){
	int me, npes;

	int wt_m = 3;
	int ht_m = 3;
	int ht_v = 3;

	int m[3][3] = {{3,2,0},{0,4,1},{2,0,1}};
	int v[] = {4,3,1};

	shmem_init();
	me = shmem_my_pe();
	npes = shmem_n_pes();

	static int count = 0;

	for (int i = 0; i < wt_m; i++){
		count += m[me][i] * v[i];
	}

	shmem_barrier_all();

	if (me == 0){
		for (int pe = 0; pe < npes; pe++){
			int ans = shmem_int_g(&count, pe);
			printf("%d\n", ans);
		}

		//printf("%d: count total: %d\n", me, total);
	}

	shmem_finalize();
	return 0;
}
