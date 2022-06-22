#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <pthread.h>

int thresh;

struct timer {
    clock_t start;
    clock_t end;
    //double diff;
    double total;
};

struct vtimer {
    struct timer main;
    struct timer add;
    struct timer mult;
    struct timer split;
};

struct vtimer timerv;

typedef struct {
    int num;
    int d;
    int **m;
    int **n;
    int **result;
} MMDATA;

MMDATA multi_data;

/*void print_matrix(int d, int **a) {
    int i,j;
    for (i = 0; i < d; i++) {
        printf("row %i: ", i);
        for (j = 0; j < d; j++) {
            printf("%d ", a[i][j]);
        }
        printf("\n");
    }
}*/


void add(int d, int **a, int **b, int **result) {
    //Add two nxn matrixs
    
    timerv.add.start = clock();

    int i,j;
    for (i = 0; i < d; i++) {
        for (j = 0; j < d; j++) {
            result[i][j] = a[i][j] + b[i][j];
        }
    }
    
    timerv.add.end = clock();
    timerv.add.total += (double)(timerv.add.end - timerv.add.start);
}

void sub(int d, int **a, int **b, int **result) {
    //Subtract two nxn 
    int i,j;
    for (i = 0; i < d; i++) {
        for (j = 0; j < d; j++) {
            result[i][j] = a[i][j] - b[i][j];
        }
    }
}

/*void mm_multi(int d, int **matrixm, int **matrixn, int **result) {
    timerv.mult.start = clock();

    int i,j,k;

    for (i = 0; i < d; i++) {
        for (j = 0; j < d; j++) {
            result[i][j] = 0;
        }
    }

    for (i = 0; i < d; i++) {
        for (j = 0; j < d; j++) {
            for (k = 0; k < d; k++) {
                result[i][j] += matrixm[i][k] * matrixn[k][j];
            }
        }
    }

    timerv.mult.end = clock();
    timerv.mult.total += (double)(timerv.mult.end - timerv.mult.start);
}*/

void *mm_multi(void *arg) {
    int i,j,k;

    int l1, l2, alt;
    int **m, **n, **result;
    int offset = (int)arg;
    
    //printf("In thread #%d\n", offset);
    
    l1 = multi_data.d;
    l2 = multi_data.num;
    alt = offset * l2;
    m = multi_data.m;
    n = multi_data.n;
    result = multi_data.result;
  
    for (i = 0 + alt; i < l2 + alt; i++) {
        for (j = 0; j < l1; j++) {
            for (k = 0; k < l1; k++) {
                result[i][j] += m[i][k] * n[k][j];
            }
        }
    }

    //printf("Exiting thread #%d\n", offset);
    pthread_exit((void*) 0);
}


void str_mm(int ht, int wt, int **matrixm, int **matrixn, int **result)
{
    if (wt <= thresh) {
        int num_threads = 4;
        pthread_t threads[num_threads];
        pthread_attr_t attr;
        void *status;
        int t;
        int hold_ht = thresh/num_threads;

        multi_data.num = hold_ht;
        multi_data.d = wt;
        multi_data.m = matrixm;
        multi_data.n = matrixn;
        multi_data.result = result;

        //printf("\nmulti_data: %d\n", multi_data.m[0][0]);
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        for(t=0; t<num_threads; t++) {
            //printf("Creating thread %d\n", t);
            pthread_create(&threads[t], &attr, mm_multi, (void *)t);
        }

        pthread_attr_destroy(&attr);

        for(t=0; t<num_threads; t++) {
            pthread_join(threads[t], &status);
        }
        
        //pthread_exit(NULL);
        
        //mm_multi(wt, matrixm, matrixn, result);
    }
    else {
        timerv.split.start = clock();

        int len = wt/2;
        
        int **A11, **A12, **A21, **A22;
        int **B11, **B12, **B21, **B22;
        
        A11 = malloc(sizeof(int*) * len);
        A12 = malloc(sizeof(int*) * len);
        A21 = malloc(sizeof(int*) * len);
        A22 = malloc(sizeof(int*) * len);

        B11 = malloc(sizeof(int*) * len);
        B12 = malloc(sizeof(int*) * len);
        B21 = malloc(sizeof(int*) * len);
        B22 = malloc(sizeof(int*) * len);

        int **C11, **C12, **C21, **C22;
        C11 = malloc(sizeof(int*) * len);
        C12 = malloc(sizeof(int*) * len);
        C21 = malloc(sizeof(int*) * len);
        C22 = malloc(sizeof(int*) * len);

        int **m1, **m2, **m3, **m4, **m5, **m6, **m7, **hold, **hold2;
        m1 = malloc(sizeof(int*) * len);
        m2 = malloc(sizeof(int*) * len);
        m3 = malloc(sizeof(int*) * len);
        m4 = malloc(sizeof(int*) * len);
        m5 = malloc(sizeof(int*) * len);
        m6 = malloc(sizeof(int*) * len);
        m7 = malloc(sizeof(int*) * len);
        hold = malloc(sizeof(int*) * len);
        hold2 = malloc(sizeof(int*) * len);
    
        int x;
        for(x = 0; x < len; x++) {
            A11[x] = malloc(sizeof(int) * len);
            A12[x] = malloc(sizeof(int) * len);
            A21[x] = malloc(sizeof(int) * len);
            A22[x] = malloc(sizeof(int) * len);
            
            B11[x] = malloc(sizeof(int) * len);
            B12[x] = malloc(sizeof(int) * len);
            B21[x] = malloc(sizeof(int) * len);
            B22[x] = malloc(sizeof(int) * len);

            m1[x] = malloc(sizeof(int) * len);
            m2[x] = malloc(sizeof(int) * len);
            m3[x] = malloc(sizeof(int) * len);
            m4[x] = malloc(sizeof(int) * len);
            m5[x] = malloc(sizeof(int) * len);
            m6[x] = malloc(sizeof(int) * len);
            m7[x] = malloc(sizeof(int) * len);
            hold[x] = malloc(sizeof(int) * len);
            hold2[x] = malloc(sizeof(int) * len);

            C11[x] = malloc(sizeof(int) * len);
            C12[x] = malloc(sizeof(int) * len);
            C21[x] = malloc(sizeof(int) * len);
            C22[x] = malloc(sizeof(int) * len);
        }

        int i, j;
        for (i = 0; i < len; i++) {
            for (j = 0; j < len; j++) {
                A11[i][j] = matrixm[i][j];
                A12[i][j] = matrixm[i][j+len];
                A21[i][j] = matrixm[i+len][j];
                A22[i][j] = matrixm[i+len][j+len];

                B11[i][j] = matrixn[i][j];
                B12[i][j] = matrixn[i][j+len];
                B21[i][j] = matrixn[i+len][j];
                B22[i][j] = matrixn[i+len][j+len];
            
                m1[i][j] = 0;
                m2[i][j] = 0;
                m3[i][j] = 0;
                m4[i][j] = 0;
                m5[i][j] = 0;
                m6[i][j] = 0;
                m7[i][j] = 0;
                hold[i][j] = 0;
                hold2[i][j] = 0;
            }
        }

        timerv.split.end = clock();
        timerv.split.total = (double)(timerv.split.end - timerv.split.start);

        //Solving M's
        add(len, A11, A22, hold);
        add(len, B11, B22, hold2);
        str_mm(len, len, hold, hold2, m1);

        add(len, A21, A22, hold);
        str_mm(len, len, hold, B11, m2);

        sub(len, B12, B22, hold);
        str_mm(len, len, A11, hold, m3);

        sub(len, B21, B11, hold);
        str_mm(len, len, A22, hold, m4);

        add(len, A11, A12, hold);
        str_mm(len, len, hold, B22, m5);

        sub(len, A21, A11, hold);
        add(len, B11, B12, hold2);
        str_mm(len, len, hold, hold2, m6);

        sub(len, A12, A22, hold);
        add(len, B21, B22, hold2);
        str_mm(len, len, hold, hold2, m7);
        
        /*print_matrix(len, m1);
        print_matrix(len, m2);
        print_matrix(len, m3);
        print_matrix(len, m4);
        print_matrix(len, m5);
        print_matrix(len, m6);
        print_matrix(len, m7);*/


        //Solving C's
        add(len, m1, m4, C11);
        sub(len, C11, m5, C11);
        add(len, C11, m7, C11);
        
        add(len, m3, m5, C12);
        
        add(len, m2, m4, C21);
        
        sub(len, m1, m2, C22);
        add(len, C22, m3, C22);
        add(len, C22, m6, C22);

        int k,l;
        for (k = 0; k < len; k++) {
            for (l = 0; l < len; l++) {
                result[k][l] = C11[k][l];
                result[k][l+len] = C12[k][l];
                result[k+len][l] = C21[k][l];
                result[k+len][l+len] = C22[k][l];
            }
        }

        for (i = 0; i < len; i++) {
            free(A11[i]);
            free(A12[i]);
            free(A21[i]);
            free(A22[i]);
            free(B11[i]);
            free(B12[i]);
            free(B21[i]);
            free(B22[i]);

            free(m1[i]);
            free(m2[i]);
            free(m3[i]);
            free(m4[i]);
            free(m5[i]);
            free(m6[i]);
            free(m7[i]);
            free(hold[i]);
            free(hold2[i]);

            free(C11[i]);
            free(C12[i]);
            free(C21[i]);
            free(C22[i]);
        }
        free(A11);
        free(A12);
        free(A21);
        free(A22);
        free(B11);
        free(B12);
        free(B21);
        free(B22);

        free(m1);
        free(m2);
        free(m3);
        free(m4);
        free(m5);
        free(m6);
        free(m7);
        free(hold);
        free(hold2);

        free(C11);
        free(C12);
        free(C21);
        free(C22);
    }
}

int * strassen_mm(int *m, int *n, int wt, int ht, int t)
{

    /*int m[] = {5,2,6,1,0,6,2,0,
               3,8,1,4,1,8,5,6,
               1,2,3,4,5,6,7,8,
               8,7,6,5,4,3,2,1,
               5,2,6,1,0,6,2,0,
               3,8,1,4,1,8,5,6,
               1,2,3,4,5,6,7,8,
               8,7,6,5,4,3,2,1};
    int n[] = {7,5,8,0,1,8,2,6,
               9,4,3,8,5,3,7,9,
               2,4,6,8,1,3,5,7,
               7,5,3,1,8,6,4,2,
               7,5,8,0,1,8,2,6,
               9,4,3,8,5,3,7,9,
               2,4,6,8,1,3,5,7,
               7,5,3,1,8,6,4,2};
    int wt = 8;
    int ht = 8;*/

    timerv.main.start = clock();

    thresh = t;

    int **matrixm;
    matrixm = malloc(sizeof(int*) * ht);
    int **matrixn;
    matrixn = malloc(sizeof(int*) * ht);
    int **result;
    result = malloc(sizeof(int*) * ht);
    int * answer = (int *)malloc(sizeof(int) * (ht * wt));

    
    int x, y;
    for(x = 0; x < ht; x++) {
        matrixm[x] = malloc(sizeof(int) * wt);
        matrixn[x] = malloc(sizeof(int) * wt);
        result[x] = malloc(sizeof(int) * wt);
    }

    //DELETE
    int count = 0;
    for (x = 0; x <ht; x++) {
        for (y = 0; y < wt; y++) {
            matrixm[x][y] = m[count];
            matrixn[x][y] = n[count];
            result[x][y] = 0;
            answer[count] = count;
            count++;
        }
    }

    str_mm(ht, wt, matrixm, matrixn, result);

    int hold = 0;
    for (x = 0; x < ht; x++) {
         for (y = 0; y < wt; y++) {
            answer[hold] = result[x][y];
            hold++;
        }
    }

    for (x = 0; x < ht; x++) {
        free(result[x]);
        free(matrixm[x]);
        free(matrixn[x]);
    }
    free(result);
    free(matrixm);
    free(matrixn);

    timerv.main.end = clock();
    timerv.main.total = (double)(timerv.main.end - timerv.main.start);

    printf("TIME MAIN: %f\n", (double)timerv.main.total/CLOCKS_PER_SEC);
    printf("TIME MULT/MAIN: %f\n", (double)timerv.mult.total/timerv.main.total);
    printf("TIME ADD/MAIN: %f\n", (double)timerv.add.total/timerv.main.total);
    printf("TIME SPLIT/MAIN: %f\n", (double)timerv.split.total/timerv.main.total);

    return answer;
}


