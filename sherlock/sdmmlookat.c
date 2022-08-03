#include <stdio.h>
#include <stdlib.h>
#include <shmem.h>
#include <time.h>
#include "sdmm.h"

int getNumRows (int me, int numSplit, int rows) {
	int x = 0;
	if (me < (rows%numSplit)) {
		x = 1;
	}

	return ((int)rows/numSplit) + x;
}

int getOffset (int me, int numSplit, int rows) {
	int x = rows % numSplit;
	if (x > me) {
		x = me;
	}
	return (me * ((int)rows/numSplit)) + x;
}

void printArray (Matrix m) {
	int i,j;
	for (i = 0; i < m.rows; i++){
		for (j = 0; j < m.cols; j++){
			printf("%d  ", (int)m.data[(i*m.cols) + j]);
		}
		printf("\n");
	}
}

void rand_Matrix (Matrix m){
	int i;
	for (int i = 0; i < (m.rows * m.cols); i++) {
		//m.data[i] = (TYPE) rand() % 2;
		m.data[i] = (TYPE) i;
	}
}

//1 is shmem_malloc, 2 is malloc
void define_Matrix (Matrix *m, int rows, int cols, int test) {
	m->rows = rows;
	m->cols = cols;

	if (test == 1) {
		m->data = shmem_malloc(sizeof(TYPE) * (rows*cols));
	}
	else {
		m->data = malloc(sizeof(TYPE) * (rows*cols));
	}
}

void split_by_cols (Matrix a, Matrix *a_split, int numSplits) {
	int i,j,k;

	for (i=0; i < numSplits; i++) {
		int numCols = getNumRows(i, numSplits, a.cols);
		int offset = getOffset(i, numSplits, a.cols);

		define_Matrix(&a_split[i], a.rows, numCols, 2);
		//a_split[i].rows = a.rows;
		//a_split[i].cols = numCols;
		//a_split[i].data = malloc(sizeof(TYPE) * (numCols * a.rows));

		for (j=0; j < numCols; j++) {
			for (k=0; k < a.rows; k++) {
				a_split[i].data[(k*numCols)+j] = a.data[(k*a.cols)+j+offset];
			}
		}
	}
}		
		

void split_by_rows (Matrix b, Matrix *b_split, int numSplits) {
	int i, j;

	for (i = 0; i < numSplits; i++) {
		int numData = getNumRows(i, numSplits, b.rows) * b.cols;
		int offset = getOffset(i, numSplits, b.rows) * b.cols;

		define_Matrix(&b_split[i], (numData/b.cols), b.cols, 2);
		//b_split[i].rows = numData/b.cols;
		//b_split[i].cols = b.cols;
		//b_split[i].data = malloc(sizeof(TYPE) * numData);

		for (j = 0; j < numData; j++) {
			b_split[i].data[j] = b.data[j+offset];
		}
	}
}

// A * B = C
void mm_multi(Matrix a, Matrix b, Matrix c) {
	int i,j,k;

	for (i = 0; i < c.rows; i++) {
		for (j = 0; j < c.cols; j++) {
			c.data[(i*c.cols)+j] = 0;
			//element(c,i,j) = 0;
		}
	}

	for (i = 0; i < a.rows; i++) {
		for (j = 0; j < b.cols; j++) {
			for (k = 0; k < a.cols; k++) {
				c.data[(i*c.cols)+j] += a.data[(i*a.cols)+k] * b.data[(k*b.cols)+j];
				//element(c,i,j) += element(a,i,k) * element(b,k,j);
			}
		}
	}
}

int main(){
	int me, npes;

	/*int ht_m = 4; //this
	int wt_m = 4;
	int ht_n = 4;
	int wt_n = 4; //this*/

	//static int m[ht_m][wt_m] = {{9,5,2},{1,8,5},{3,1,6}};
	//static int n[ht_n][wt_n] = {{3,2},{1,4},{5,3}};

	//int m[2][3] = {{8,1,2},{-5,6,7}};
	//int n[3][2] = {{-5,1},{0,2},{-11,7}};

	//int m[2][3] = {{2,1,4},{0,1,1}};
	//int n[3][4] = {{6,3,-1,0},{1,1,0,4},{-2,5,0,2}};

	shmem_init();
	me = shmem_my_pe();
	npes = shmem_n_pes();

	//int avgNumRows = getNumRows(0, npes, ht_m);
	int numSplits = 3;
	int numPrevent = 2;

	//printf("avgNumRows: %d\n", avgNumRows);
	//printf("avgNumRows1: %d\n", numRows[0]);
	//printf("avgNumRows2: %d\n", numRows[1]);

	/*Matrix a;
	a.rows = 4; //this
	a.cols = 5;
	a.data = shmem_malloc(sizeof(TYPE) * (a.rows*a.cols));
	Matrix b;
	b.rows = 5;
	b.cols = 4; //this
	b.data = shmem_malloc(sizeof(TYPE) * (b.rows*b.cols));*/

	Matrix a;
	Matrix b;
	Matrix * a_split;
	Matrix * b_split;

	Matrix A_t;
	Matrix B_t;

	static int A_rows, A_cols;
	static int B_rows, B_cols;

	/*int **matrixm;
	matrixm = shmem_malloc(sizeof(int*) * avgNumRows);
	int **matrixn;
	matrixn = shmem_malloc(sizeof(int*) * ht_n);
	int **result;
	result = shmem_malloc(sizeof(int*) * avgNumRows);
	int **ans;
	ans = shmem_malloc(sizeof(int*) * ht_m);*/

	/*int x;
	for(x = 0; x < avgNumRows; x++) {
		matrixm[x] = shmem_malloc(sizeof(int) * wt_m);
		result[x] = shmem_malloc(sizeof(int) * wt_n);
	}
	for(x = 0; x < ht_n; x++) {
		//matrixn[x] = shmem_malloc(sizeof(int) * wt_n);
		n[x] = shmem_malloc(sizeof(int) * wt_n);
	}
	for(x = 0; x < ht_m; x++) {
		//ans[x] = shmem_malloc(sizeof(int) * wt_n);
		m[x] = shmem_malloc(sizeof(int) * wt_m);
	}
	//printf("%d: FINSIHED MALLOC\n", me);*/

	/*static int matrixn[ht_n][wt_n];
	static int matrixm[numRows][wt_m];
	static int result[numRows][wt_n];*/

	if (me == 0) {
		//Matrix a;
		define_Matrix(&a, 4, 7, 2);
		//a.rows = 4; //this
		//a.cols = 4;
		//a.data = malloc(sizeof(TYPE) * (a.rows*a.cols));
		//Matrix b;
		define_Matrix(&b, 7, 4, 2);
		//b.rows = 4;
		//b.cols = 4; //this
		//b.data = malloc(sizeof(TYPE) * (b.rows*b.cols));

		//printf("SIZE A: %d\n", a.rows);

		srand(time(0)+me);
		rand_Matrix(a);
		rand_Matrix(b);

		printf("ARRAY A: \n");
		printArray(a);
		printf("\nARRAY B: \n");
		printArray(b);

		//Split A by cols and B by rows
		//Matrix * a_split;
		//Matrix * b_split;

		a_split = malloc(sizeof(Matrix) * numSplits);
		b_split = malloc(sizeof(Matrix) * numSplits);

		split_by_cols(a, a_split, numSplits);
		split_by_rows(b, b_split, numSplits);	

		printf("\nSPLIT B[0]: \n");
		printArray(b_split[0]);
		/*printf("\nSPLIT B[1]: \n");
		printArray(b_split[1]);
		printf("\nSPLIT B[2]: \n");
		printArray(b_split[2]);

		printf("\nSPLIT A[0]: \n");
		printArray(a_split[0]);
		printf("\nSPLIT A[1]: \n");
		printArray(a_split[1]);
		printf("\nSPLIT A[2]: \n");
		printArray(a_split[2]);*/

		int i;
		for (i = 0; i < numSplits; i++) {
			shmem_int_p(&A_rows, a_split[i].rows, i+1);
			shmem_int_p(&A_cols, a_split[i].cols, i+1);

			shmem_int_p(&B_rows, b_split[i].rows, i+1);
			shmem_int_p(&B_cols, b_split[i].cols, i+1);
		}
	}

	shmem_barrier_all();

	//printf("%d: A_rows: %d  A_cols: %d\n", me, A_rows, A_cols);
	//printf("%d: B_rows: %d  B_cols: %d\n", me, B_rows, B_cols);

	define_Matrix(&A_t, A_rows, A_cols, 1);
	define_Matrix(&B_t, B_rows, B_cols, 1);

	//A_t.rows = A_rows;
	//A_t.cols = A_cols;
	//A_t.data = (TYPE*)shmem_malloc(sizeof(TYPE) * (A_rows*A_cols));

	//B_t.rows = B_rows;
	//B_t.cols = B_cols;
	//B_t.data = (TYPE*)shmem_malloc(sizeof(TYPE) * (B_rows*B_cols));

	shmem_barrier_all();
	//printf("%d: A rows: %d   A cols: %d\n", me, A_t.rows, A_t.cols);
	//printf("%d: B rows: %d   B cols: %d\n", me, B_t.rows, B_t.cols);

	if (me == 0) {
		//Create random R's and S's
		Matrix * randomR;
		Matrix * randomS;
		randomR = malloc(sizeof(Matrix) * numPrevent);
		randomS = malloc(sizeof(Matrix) * numPrevent);

		int i;
		for (i = 0; i < numPrevent; i++) {
			define_Matrix(&randomR[i], a.rows, ((int)a.cols/numSplits), 2);
			rand_Matrix(randomR[i]);

			define_Matrix(&randomS[i], ((int)b.rows/numSplits), b.cols, 2);
			rand_Matrix(randomS[i]);
		}

		//printf("\nRANDR[0]: \n");
		//printArray(randomR[0]);
		//printf("\nRANDR[1]: \n");
		//printArray(randomR[1]);

		//printf("\nRANDS[0]: \n");
		//printArray(randomS[0]);
		//printf("\nRANDS[1]: \n");
		//printArray(randomS[1]);



		//INSERT TIFFANY CODE HERE



		//printf("AHHH\n");
		//printArray(b_split[0]);
		//printf("NOOO: %d\n", (b_split[0].rows*b_split[0].cols));
		shmem_T_put(&B_t.data[2], b_split[1].data, (b_split[1].rows*b_split[1].cols), 2);
		//shmem_T_put(&B_t.data[7], &b_split[0].data[1],1, 1);
		//printf("CHECK: %d\n", b_split[0].data[1]);
		//shmem_barrier_all();
		//printf("B_t: %p\n", B_t.data);
		//printf("B_t rows: %p\n", &B_t.rows);
		printf("A_t [4]: %p\n", &A_t.data[4]);
		printf("A_t: %p\n", A_t.data);

		for (i = 0; i < numSplits; i++) {
			//printf("I: %d\n", i);
			//printArray(b_split[i]);
			//printf("rows: %d\n", b_split[i].rows);
			//printf("cols: %d\n", b_split[i].cols);
			//printf("ANS: %d\n", b_split[i].rows*b_split[i].cols);
			printf("b_split: %p\n", b_split[i].data);
			printf("b_split2: %p\n", &b_split[i]);
			
			//shmem_T_put(B_t.data, b_split[i].data, (b_split[i].rows*b_split[i].cols), (i+1));
			shmem_T_put(A_t.data, a_split[i].data, (a_split[i].rows*a_split[i].cols), (i+1));
			//printf("A DONE\n");
			//shmem_T_put(B_t.data, b_split[i].data, (b_split[i].rows*b_split[i].cols), (i+1));
		}
		/*for (i = 0; i < numSplits; i++) {
			free(a_split[i].data);
			free(b_split[i].data);
		}*/
		for (i = 0; i < numPrevent; i++) {
			free(randomR[i].data);
			free(randomS[i].data);
		}
		//free(a_split);
		//free(b_split);
		free(randomR);
		free(randomS);
		free(a.data);
		free(b.data);
	}

	/*int i,j,k;
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

	shmem_barrier_all();

	if (me == 0){
		printf("\nRESULT: %d by %d \n", ht_m, wt_n);
		printArray(ans, 2, wt_n);
	}
	
	//printf("%d: STARTING FREE\n", me);

	for(x = 0; x < avgNumRows; x++) {
		shmem_free(matrixm[x]);
		shmem_free(result[x]);
	}*/

	/*if (me == 0) {
		for(x = 0; x < ht_n; x++) {
			//shmem_free(matrixn[x]);
			shmem_free(n[x]);
		}
		for(x = 0; x < ht_m; x++) {
			//shmem_free(ans[x]);
			shmem_free(m[x]);
		}
		shmem_free(result);
		shmem_free(matrixm);
		shmem_free(matrixn);
		shmem_free(ans);
		shmem_free(a.data);
		shmem_free(b.data);
	}*/

	shmem_barrier_all();

	printf("%d: A_t: \n", me);
	printArray(A_t);

	printf("%d: B_t: \n", me);
	printArray(B_t);
	printf("B_t.data[0]: %d\n", B_t.data[0]);

	if (me == 1){
		printf("Bptr: %p  %p\n", &B_t.data[0], &B_t.data[1]);
	}

	shmem_barrier_all();

	shmem_free(A_t.data);
	shmem_free(B_t.data);

	shmem_finalize();
	return 0;
}

