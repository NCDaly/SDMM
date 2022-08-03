#include <stdio.h>
#include <stdlib.h>
#include <shmem.h>

int main(){
	int me, npes;

	int ht_m = 2; //this
	int wt_m = 3;
	int ht_n = 3;
	int wt_n = 4; //this

	//int m[3][3] = {{9,5,2},{1,8,5},{3,1,6}};
	//int n[3][2] = {{3,2},{1,4},{5,3}};

	//int m[2][3] = {{8,1,2},{-5,6,7}};
	//int n[3][2] = {{-5,1},{0,2},{-11,7}};

	int m[2][3] = {{2,1,4},{0,1,1}};
	int n[3][4] = {{6,3,-1,0},{1,1,0,4},{-2,5,0,2}};

	shmem_init();
	me = shmem_my_pe();
	npes = shmem_n_pes();

	//ans = ht_m, wt_n
	static int count = 0;

	for (int i = 0; i < wt_m; i++){
		count += m[(int)me/wt_n][i] * n[i][me%wt_n];
	}

	/*for (int i = 0; i < ht_m; i++) {
		for (int j = 0; j < wt_n; j++) {
			for (int k = 0; k < wt_m; ++k) {
				count += m[i][k] * m[k][j];
			}
		}
	}*/


	shmem_barrier_all();

	if (me == 0){
		for (int pe = 0; pe < npes; pe++){
			int ans = shmem_int_g(&count, pe);
			printf("%d ", ans);
			
			if (pe%wt_n == (wt_n-1)){
				printf("\n");
			}
		}

		//printf("%d: count total: %d\n", me, total);
	}

	shmem_finalize();
	return 0;
}
