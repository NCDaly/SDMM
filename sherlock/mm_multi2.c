#include <stdio.h>
#include <stdlib.h>
#include <shmem.h>
#include <time.h>

int getNumRows (int me, int npes, int ht) {
	int x = 0;
	if (me < (ht%npes)) {
		x = 1;
	}

	return ((int)ht/npes) + x;
}

int getOffset (int me, int npes, int ht) {
	int x = ht % npes;
	if (x > me) {
		x = me;
	}
	return (me * ((int)ht/npes)) + x;
}

void printArray (int **a, int ht, int wt) {
	int i,j;
	for (i = 0; i < ht; i++){
		for (j = 0; j < wt; j++){
			printf("%d  ", a[i][j]);
		}
		printf("\n");
	}
}

int main(){
	int me, npes;

	int ht_m = 1024; //this
	int wt_m = 1024;
	int ht_n = 1024;
	int wt_n = 1024; //this

	//static int m[ht_m][wt_m] = {{9,5,2},{1,8,5},{3,1,6}};
	//static int n[ht_n][wt_n] = {{3,2},{1,4},{5,3}};

	//int m[2][3] = {{8,1,2},{-5,6,7}};
	//int n[3][2] = {{-5,1},{0,2},{-11,7}};

	//int m[2][3] = {{2,1,4},{0,1,1}};
	//int n[3][4] = {{6,3,-1,0},{1,1,0,4},{-2,5,0,2}};

	shmem_init();
	me = shmem_my_pe();
	npes = shmem_n_pes();

	/*int **m;
	m = shmem_malloc(sizeof(int*) * ht_m);
	int **n;
	n = shmem_malloc(sizeof(int*) * ht_n);

	int x;
	for (x = 0; x < ht_m; x++) {
		m[x] = shmem_malloc(sizeof(int) * wt_m);
	}
	for (x = 0; x < ht_n; x++) {
		n[x] = shmem_malloc(sizeof(int) * wt_n);
	}

	if (me == 0) {
		srand(time(0)+me);

		int i,j;
		for (int i = 0; i < ht_m; i++) {
			for (int j = 0; j < wt_m; j++) {
				m[i][j] = (int) rand() % 2;
			}
		}

		for (int i = 0; i < ht_m; i++) {
			for (int j = 0; j < wt_m; j++) {
				n[i][j] = (int) rand() % 2;;
			}
		}

		//printf("ARRAY M: \n");
		//printArray(m, ht_m, wt_m);
		//printf("\nARRAY N: \n");
		//printArray(n, ht_n, wt_n);
	}*/

	int avgNumRows = getNumRows(0, npes, ht_m);

	//printf("avgNumRows: %d\n", avgNumRows);
	//printf("avgNumRows1: %d\n", numRows[0]);
	//printf("avgNumRows2: %d\n", numRows[1]);

	int **m;
	m = shmem_malloc(sizeof(int*) * ht_m);
	int **n;
	n = shmem_malloc(sizeof(int*) * ht_n);
	int **matrixm;
	matrixm = shmem_malloc(sizeof(int*) * avgNumRows);
	int **matrixn;
	matrixn = shmem_malloc(sizeof(int*) * ht_n);
	int **result;
	result = shmem_malloc(sizeof(int*) * avgNumRows);
	int **ans;
	ans = shmem_malloc(sizeof(int*) * ht_m);

	int x;
	for(x = 0; x < avgNumRows; x++) {
		matrixm[x] = shmem_malloc(sizeof(int) * wt_m);
		result[x] = shmem_malloc(sizeof(int) * wt_n);
	}
	for(x = 0; x < ht_n; x++) {
		matrixn[x] = shmem_malloc(sizeof(int) * wt_n);
		n[x] = shmem_malloc(sizeof(int) * wt_n);
	}
	for(x = 0; x < ht_m; x++) {
		ans[x] = shmem_malloc(sizeof(int) * wt_n);
		m[x] = shmem_malloc(sizeof(int) * wt_m);
	}
	//printf("%d: FINSIHED MALLOC\n", me);

	/*static int matrixn[ht_n][wt_n];
	static int matrixm[numRows][wt_m];
	static int result[numRows][wt_n];*/

	if (me == 0) {
		srand(time(0)+me);

		int i,j;
		for (int i = 0; i < ht_m; i++) {
			for (int j = 0; j < wt_m; j++) {
				m[i][j] = (int) rand() % 2;
			}
		}

		for (int i = 0; i < ht_m; i++) {
			for (int j = 0; j < wt_m; j++) {
				n[i][j] = (int) rand() % 2;;
			}
		}

		/*printf("ARRAY M: \n");
		printArray(m, ht_m, wt_m);
		printf("\nARRAY N: \n");
		printArray(n, ht_n, wt_n);*/
	}

	/*if (me == 0){
		//printf("%d: IN 0 IF\n", me);
		int sum = 0;

		for (int pe = 1; pe < npes; pe++){
			int i,j;
			for (i = 0; i < numRows[pe-1]; i++) {
				shmem_int_put(matrixm[i], m[i+sum], wt_m, pe);
			}
			sum += numRows[pe-1];

			//printf("FILLED M_HOLD: m_hold(0,0): %d\n", m_hold[0]);

			for (i = 0; i < ht_n; i++) {
				shmem_int_put(matrixn[i], n[i], wt_n, pe);
			}
			//shmem_int_put(matrixm, m_hold, (numRows*wt_m), pe);
			//printf("DONE WITH PUT\n");
		}
	}*/

	//shmem_barrier_all();

	//if (me != 0) {
		//printf("%d: n[0][0]: %d\n", me, n[0][0]);
		//printf("%d: matrixm[1][0]: %d\n", me, matrixm[1][0]);
		//printf("%d: matrixn[0][1]: %d\n", me, matrixn[0][1]);
		//printf("%d: matrixn[1][1]: %d\n", me, matrixn[1][1]);

	int i,j,k;
	int numRows = getNumRows(me, npes, ht_m);
	int offset = getOffset(me, npes, ht_m);
	//printf("%d: numRowsI: %d\n", me, numRowsI);
	//printf("%d: offset: %d\n", me, offset);

	for (i = 0; i < ht_n; i++) {
		//printf("i is %d\n", i);
		shmem_int_get(matrixn[i], n[i], wt_n, 0);
	}

	for (i = 0; i < numRows; i++) {
		shmem_int_get(matrixm[i], m[i+offset], wt_m, 0);
	}


	for (i = 0; i < numRows; i++) {
		for (j = 0; j < wt_n; j++) {
			result[i][j] = 0;
		}
	}

	for (i = 0; i < numRows; i++) {
		for (j = 0; j < wt_n; j++) {
			for (k = 0; k < ht_n; k++) {
				result[i][j] += matrixm[i][k] * matrixn[k][j];
			}
		}
	}

	for(i = 0; i < numRows; i++) {
		shmem_int_put(ans[i+offset], result[i], wt_n, 0);
	}
	//}

	shmem_barrier_all();

	if (me == 0){
		/*int ans[ht_m][wt_n];
		int sum = 0;
		int i,j;
		for (int pe = 1; pe < npes; pe++){
			//int a_hold[numRows][wt_m];

			for(i = 0; i < numRows[pe-1]; i++) {
				shmem_int_get(ans[i+sum], result[i], wt_n, pe);
			}

			sum += numRows[pe-1];
			//shmem_int_get(a_hold, result, (numRows*wt_m), pe);

			for (i = 0; i < numRows; i++) {
				for (j = 0; j < wt_m; j++) {
					ans[i+(pe*numRows)][j] = a_hold[i][j];
				}
			}
		}*/

		printf("\nRESULT: %d by %d \n", ht_m, wt_n);
		printArray(ans, 2, wt_n);
	}
	
	//printf("%d: STARTING FREE\n", me);

	for(x = 0; x < avgNumRows; x++) {
		shmem_free(matrixm[x]);
		shmem_free(result[x]);
	}
	for(x = 0; x < ht_n; x++) {
		shmem_free(matrixn[x]);
		shmem_free(n[x]);
	}
	for(x = 0; x < ht_m; x++) {
		shmem_free(ans[x]);
		shmem_free(m[x]);
	}
	shmem_free(result);
	shmem_free(matrixm);
	shmem_free(matrixn);
	shmem_free(ans);
	shmem_free(n);
	shmem_free(m);

	shmem_finalize();
	return 0;
}

