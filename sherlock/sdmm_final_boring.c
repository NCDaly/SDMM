#include <stdio.h>
#include <stdlib.h>
#include <shmem.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "mersenne_prime_field.h"
#include "sdmm.h"
#include "interpolate_matrix.h"
#include "vandermonde_inverter.h"

struct timer {
	clock_t start;
	clock_t end;
	double total;
};

//Returns the minimum number of the a and b provided
int min (int a, int b) {
        return (a > b) ? b : a;
}

//Returns the number of rows a specific node/submatrix recieves based on total number of rows and number of nodes/matrices to split between
int getNumRows (int me, int numSplit, int rows) {
	int x = 0;
	if (me < (rows%numSplit)) {
		x = 1;
	}

	return ((int)rows/numSplit) + x;
}

//Returns the distance from the start of original matrix to split for a specific node/submatrix based upon total number of rows and number of nodes/matrices to split between
int getOffset (int me, int numSplit, int rows) {
	int x = rows % numSplit;
	if (x > me) {
		x = me;
	}
	return (me * ((int)rows/numSplit)) + x;
}

//Prints out matrix
void printArray (Matrix m) {
	int i,j;
	for (i = 0; i < m.rows; i++){
		for (j = 0; j < m.cols; j++){
			printf("%3d  ", (int)m.data[(i*m.cols) + j]);
		}
		printf("\n");
	}
}

//Fills a matrix with randomly generated numbers
void rand_Matrix (Matrix m){
	int i;
	for (int i = 0; i < (m.rows * m.cols); i++) {
		m.data[i] = (TYPE) mpf_mod(rand());
	}
}

//Sets the matrices .rows, .cols, and mallocs enough space for .data
//If test is 1, the .data is shmem_malloced. If test is 2, .data is is malloced
void define_Matrix (Matrix *m, int rows, int cols, int test) {
	m->rows = rows;
	m->cols = cols;

	if (test == 1) {
		m->data = shmem_calloc((rows*cols), sizeof(TYPE));
	}
	else {
		m->data = calloc((rows*cols), sizeof(TYPE));
	}
}

//Define matrix using data from imported file
void createStartMatrix (Matrix *m, FILE *fptr) {
	if (fptr == NULL) {
		printf("Error!");
		exit(1);
	}

	int rows, cols;
	fscanf(fptr, "%d", &rows);
	fscanf(fptr, "%d", &cols);

	define_Matrix(m, rows, cols, 2);

	int num;
	int i = 0;
	while (!feof (fptr)) {
		fscanf(fptr, "%d", &num);
		m->data[i] = (TYPE) mpf_mod(num);
		i++;
	}
}

//Splits matrix a vertically (by columns) into numSplit submatrices that are saved into a_split. a_split[0] is the first submatrix, a_split[1] is the second, ...
//Splits using block distribution. Therefore, a_split[0] will always have the greatest number of cols
void split_by_cols (Matrix a, Matrix *a_split, int numSplits) {
	int i,j,k;

	int avgCols = getNumRows(0, numSplits, a.cols);

	for (i=0; i < numSplits; i++) {
		int numCols = getNumRows(i, numSplits, a.cols);
		int offset = getOffset(i, numSplits, a.cols);

		define_Matrix(&a_split[i], a.rows, avgCols, 2);

		for (j=0; j < numCols; j++) {
			for (k=0; k < a.rows; k++) {
				a_split[i].data[(k*avgCols)+j] = a.data[(k*a.cols)+j+offset];
			}
		}
	}
}		
		
//Splits matrix b horizontally (by rows) into numSplit submatrices that are saved into b_split. a_split[0] is the first submatrix, a_split[1] is the second, ...
//Splits using block distribution. Therefore, b_split[0] will always have the greatest number of rowss
void split_by_rows (Matrix b, Matrix *b_split, int numSplits) {
	int i, j;

	int avgRows = getNumRows(0, numSplits, b.rows);

	for (i = 0; i < numSplits; i++) {
		int numData = getNumRows(i, numSplits, b.rows) * b.cols;
		int offset = getOffset(i, numSplits, b.rows) * b.cols;

		define_Matrix(&b_split[i], avgRows, b.cols, 2);

		for (j = 0; j < numData; j++) {
			b_split[i].data[j] = b.data[j+offset];
		}
	}
}

//Multiplies matrix A by matrix B and saves it in matrix C. Uses blocking
void mm_multi(Matrix a, Matrix b, Matrix c) {
        int i,j,k,kk,jj, ii;
        int bSize = 16;

        for (ii = 0; ii < a.rows; ii += bSize) {
          for (jj = 0; jj < b.cols; jj += bSize) {
            for (kk = 0; kk < a.cols; kk += bSize) {
              for (i = ii; i < min(a.rows, ii+bSize); i++) {
                for (j = jj; j < min(b.cols, jj + bSize); j++) {
                  TYPE sum = *element(&c,i,j);
                  for (k = kk; k < min(a.cols, kk + bSize); k++) {
                    int term = mpf_mul(*element(&a, i, k), *element(&b, k, j));
                    sum = mpf_add(sum, term);
                  }
                  *element(&c,i,j) = sum;
                }
              }
            }
          }
        }
}

//Creates polynomial h from array of split matrix
void make_mat_dot_poly(Matrix* A, Matrix* R, int terms, Polynomial* PoloG, Matrix* ptr, int* expArray, int size) {
	(*PoloG).terms = terms;
	for (int i = 0; i < terms; i++) {
		expArray[i] = i; 
	}
	(*PoloG).exponents = expArray;

	for (int i = 0; i < size; i++ ) { 
		ptr[i].rows = A[i].rows;
		ptr[i].cols = A[i].cols;
		ptr[i].data = A[i].data;  
	} 

	int dell = 0;
	for (int i = size; i < terms; i++ ) { 
		ptr[i].rows = R[dell].rows;
		ptr[i].cols = R[dell].cols;
		ptr[i].data = R[dell].data; 
		dell++; 
	}

	(*PoloG).coeffs = ptr;
}

//Creates polynomial g from array of split matrix
void make_mat_dot_poly_yuh(Matrix* A, Matrix* R, int terms, Polynomial* PoloG, Matrix* ptr, int* expArray, int size) {
	(*PoloG).terms = terms;
	for (int i = 0; i < terms; i++) {
		expArray[i] = i; 
	}
	(*PoloG).exponents = expArray;

	int hold = 0;
	for (int i = size - 1; i >= 0; i-- ) { 
		ptr[hold].rows = A[i].rows;
		ptr[hold].cols = A[i].cols;
		ptr[hold].data = A[i].data; 
		hold++; 
	}
 
	int dell = 0;
	for (int i = size; i < terms; i++ ) { 
		ptr[i].rows = R[dell].rows;
		ptr[i].cols = R[dell].cols;
		ptr[i].data = R[dell].data; 
		dell++; 
	}
	(*PoloG).coeffs = ptr;
}

//Evaluates polynomial for specific alpha and returns resulting array
void eval_poly(int alpha, Polynomial p, Matrix* firstClass) {
	int pow_alpha[p.terms];
	pow_alpha[0] = 1;
	pow_alpha[1] = alpha;
	int i, j;
	for (j = 2; j < p.terms; j++) {
		pow_alpha[j] = mpf_mul(pow_alpha[j-1], alpha);
	}

	for (i = 0; i < ((*firstClass).rows*(*firstClass).cols); i++) {
		(*firstClass).data[i] = 0;

		for (j = 0; j < p.terms; j++) {
			Matrix m = (Matrix) p.coeffs[j];
			int term = mpf_mul(m.data[i], pow_alpha[j]);
			(*firstClass).data[i] = mpf_add((*firstClass).data[i], term);
		}
	}
}

int main(int argc, char *argv[]){
	int me, npes;

	//Declare pSync for Broadcast
	double pWrk[SHMEM_REDUCE_MIN_WRKDATA_SIZE];
	long pSync[SHMEM_BCAST_SYNC_SIZE];
	int x;
	for (x = 0; x < SHMEM_BCAST_SYNC_SIZE; x++) {
		pSync[x] = SHMEM_SYNC_VALUE;
	}

	shmem_init();
	me = shmem_my_pe();
	npes = shmem_n_pes();

	//Number of times we split the Matrix and number of colluding nodes to prevent
	int numSplits = atoi(argv[1]); //15
	int numPrevent = atoi(argv[2]); //10
	//int size = atoi(argv[3]);
	int numTerms = numSplits + numPrevent;

	char* fileA = argv[3];
	char* fileB = argv[4];
	char* fileAns = argv[5];

	//Flags to indicate when section is complete
	static int flagStart = 0;
	static int flagDone = 0;
	static uint64_t PE_done = 0;

	//Globally declared variables
	Matrix a;
	Matrix b;
	Matrix * a_split;
	Matrix * b_split;
	Matrix * c;
	c = shmem_malloc(sizeof(Matrix) * npes);

	Matrix A_t;
	Matrix B_t;

	static int arrInfo[4]; //Arows, Acols, Brows, Bcols

	//Declare timers
	struct timer main;
	main.total = 0;
	struct timer calcPoly;
	calcPoly.total = 0;
	struct timer sendingData;
	sendingData.total = 0;
	static struct timer multi;
	multi.total = 0;
	struct timer interpolate;
	interpolate.total = 0;

	//Define, fill, and split A & B. Then, fill arrInfo with the rows/cols of subarrays
	if (me == 0) {
		main.start = clock();

		//Define matrix A & B
		FILE *matrixA;
		matrixA = fopen(fileA, "r");
		createStartMatrix(&a, matrixA);
		fclose(matrixA);

		FILE *matrixB;
		matrixB = fopen(fileB, "r");
		createStartMatrix(&b, matrixB);
		fclose(matrixB);

		//Fill A & B with random numbers
		srand(time(0)+me);
		//rand_Matrix(a);
		//rand_Matrix(b);

		//Print out A & B
		/*printf("ARRAY A: \n");
		printArray(a);
		printf("\nARRAY B: \n");
		printArray(b);*/

		//Split A by cols and B by rows
		a_split = malloc(sizeof(Matrix) * numSplits);
		b_split = malloc(sizeof(Matrix) * numSplits);

		split_by_cols(a, a_split, numSplits);
		split_by_rows(b, b_split, numSplits);

		//Put the length info for subarrays on all nodes
		arrInfo[0] = a_split[0].rows;
		arrInfo[1] = a_split[0].cols;
		arrInfo[2] = b_split[0].rows;
		arrInfo[3] = b_split[0].cols;
	}

	shmem_barrier_all();

	//Broadcast subarray lengths to all nodes
	shmem_broadcast32(arrInfo, arrInfo, 4, 0, 0, 0, npes, pSync);

	//Use given subarray lengths to define A_t, B_t, and c
	define_Matrix(&A_t, arrInfo[0], arrInfo[1], 1);
	define_Matrix(&B_t, arrInfo[2], arrInfo[3], 1);

	for (x = 1; x < npes; x++) {
		define_Matrix(&c[x], arrInfo[0], arrInfo[3], 1);
	}

	//Define and fill random matrices. Put subarrays on their correct nodes. Collect all results from nodes in the order they finished computation. Use results to get answer matrix.
	if (me == 0) {
		//Create random R's and S's
		Matrix * randomR;
		Matrix * randomS;
		randomR = malloc(sizeof(Matrix) * numPrevent);
		randomS = malloc(sizeof(Matrix) * numPrevent);

		//Fill random R's and S's with random numbers
		int i,j;
		for (i = 0; i < numPrevent; i++) {
			define_Matrix(&randomR[i], a.rows, arrInfo[1], 2);
			rand_Matrix(randomR[i]);

			define_Matrix(&randomS[i], arrInfo[2], b.cols, 2);
			rand_Matrix(randomS[i]);
		}

		//Create both f & g polynomials
		int expArray[numTerms];
		Matrix *ptrf;
		ptrf = malloc(sizeof(Matrix) * numTerms);
		Polynomial f;
		make_mat_dot_poly(a_split, randomR, numTerms, &f, ptrf, expArray, numSplits);

		int expArrayg[numTerms];
		Matrix* ptrg;
		ptrg = malloc(sizeof(Matrix) * numTerms);
		Polynomial g;
		make_mat_dot_poly_yuh(b_split, randomS, numTerms, &g, ptrg, expArrayg, numSplits);

		//Define holder matrices, Ai & Bi
		Matrix Ai, Bi;
		define_Matrix(&Ai, a_split[0].rows, a_split[0].cols, 2);
		define_Matrix(&Bi, b_split[0].rows, b_split[0].cols, 2);

		for (i = 1; i < npes; i++) {
			//Create and put Ai on correct node
			calcPoly.start = clock();
			eval_poly(i, f, &Ai);
			calcPoly.end = clock();
			calcPoly.total += (double)(calcPoly.end - calcPoly.start);

			sendingData.start = clock();
			shmem_T_put(A_t.data, Ai.data, (Ai.rows*Ai.cols), i);
			sendingData.end = clock();
			sendingData.total += (double)(sendingData.end - sendingData.start);

			//Create and put Bi on correct node
			calcPoly.start = clock();
			eval_poly(i, g, &Bi);
			calcPoly.end = clock();
			calcPoly.total += (double)(calcPoly.end - calcPoly.start);

			sendingData.start = clock();
                        shmem_T_put(B_t.data, Bi.data, (Bi.rows*Bi.cols), i);
                        sendingData.end = clock();
                        sendingData.total += (double)(sendingData.end - sendingData.start);

			//Set start flag on node to start
			shmem_int_p(&flagStart, 1, i);
		}

		//Initilize Matrix Inverter
		int finalTerms = 2 * numTerms - 1;
		VdmInverter *inv = vdm_inv_init(finalTerms, (npes-1));

		//Accept enough matrices from nodes to create/solve the final polyinomial h
		while(inv->prog < inv->size) {
			shmem_int64_wait_until(&PE_done, SHMEM_CMP_NE, 0);
			for (i = 1; i < npes; i++) {

				if (PE_done & (1LU << i)) {
					vdm_inv_update(inv, i);
					PE_done -= 1LU<<i;
				}
			}
		}

		//Use h to find answer matrix
		interpolate.start = clock();
		Matrix *answer = interpolate_matrix_mat_dot(inv, c, (numSplits-1));
		interpolate.end = clock();
		interpolate.total += (double)(interpolate.end - interpolate.start);

		//Print answer matrix to file
		FILE *fanswer;
		fanswer = fopen(fileAns, "w");
		if(fanswer == NULL) {
			printf("Error!");
			exit(1);
		}

		fprintf(fanswer, "%d\n", answer->rows);
		fprintf(fanswer, "%d\n", answer->cols);
		for (i = 0; i < answer->rows; i++){
			for (j = 0; j < answer->cols; j++){
				fprintf(fanswer, "%3d  ", (int)answer->data[(i*answer->cols) + j]);
			}
			fprintf(fanswer, "\n");
		}

		fclose(fanswer);

		//Free inverter data
		vdm_inv_destroy(inv);
		destroy_matrix(answer);

		main.end = clock();
		main.total = (double)(main.end - main.start);

		//Print out timer data
		printf("TIME MAIN: %f\n", (double)main.total/CLOCKS_PER_SEC);
		printf("TIME CALCPOLY: %f\n", (double)calcPoly.total/CLOCKS_PER_SEC);
		printf("TIME SENDING DATA: %f\n", (double)sendingData.total/CLOCKS_PER_SEC);
		printf("TIME INTERPOLATE: %f\n", (double)interpolate.total/CLOCKS_PER_SEC);
		printf("TIME CALCPOLY/MAIN: %f\n", (double)calcPoly.total/main.total);
                printf("TIME SENDING DATA/MAIN: %f\n", (double)sendingData.total/main.total);
		printf("TIME INTERPOLATE/MAIN: %f\n", (double)interpolate.total/main.total);

		//Free everything malloced in PE0
		for (i = 0; i < numSplits; i++) {
                        free(a_split[i].data);
                        free(b_split[i].data);
                }
                for (i = 0; i < numPrevent; i++) {
                        free(randomR[i].data);
                        free(randomS[i].data);
                }
                free(a_split);
                free(b_split);
                free(randomR);
                free(randomS);
                free(a.data);
                free(b.data);
                free(ptrf);
                free(ptrg);
		free(Ai.data);
		free(Bi.data);
	}

	//Wait until node recieves data from PE0, then multiply both matrices. Put result back onto PE0
	if (me != 0) {
		//Prevent node from starting until start flag is set
		shmem_int_wait_until(&flagStart, SHMEM_CMP_NE, 0);

		//Define answer matrix
		Matrix C_t;
		define_Matrix(&C_t, A_t.rows, B_t.cols, 2);

		//Multiply matrices
		multi.start = clock();
                mm_multi(A_t, B_t, C_t);
                multi.end = clock();
                multi.total += (double)(multi.end - multi.start);

		//Put C_t back on node 0 and identify the node that finsihed in PE_done
		shmem_T_put(c[me].data, C_t.data, (C_t.rows*C_t.cols), 0);
		shmem_int64_atomic_or(&PE_done, 1LU<<me, 0);

		//Free data
		free(C_t.data);
	}

	//Calculate and print timer for matrix multiplication
	shmem_double_sum_to_all(&multi.total, &multi.total, 1, 0, 0, npes, pWrk, pSync);
	multi.total = multi.total/(npes-1);
	if (me == 0) {
		printf("TIME MULTI: %f\n", (double)multi.total/CLOCKS_PER_SEC);
		printf("TIME MULTI/MAIN: %f\n", (double)multi.total/main.total);
	}

	//Free all shmem_malloced data
	shmem_free(A_t.data);
	shmem_free(B_t.data);
	for(x = 1; x < npes; x++) {
		shmem_free(c[x].data);
	}
	shmem_free(c);

	shmem_finalize();
	return 0;
}

