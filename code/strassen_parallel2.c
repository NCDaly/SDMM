#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <pthread.h>

int thresh;

struct MMDATA{
    int id;
    int len;
    int **mm1;
    int **mm2;
    int **mm3;
    int **mm4;
    int **m_result;
    double *time;
};

void str_mm(int ht, int wt, int **matrixm, int **matrixn, int **result, double *time);

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

void add(int d, int **a, int **b, int **result, double *time) {
    //Add two nxn matrixs
    
    //timerv.add.start = clock();
    clock_t a_start = clock();

    int i,j;
    for (i = 0; i < d; i++) {
        for (j = 0; j < d; j++) {
            result[i][j] = a[i][j] + b[i][j];
        }
    }
    
    clock_t a_end = clock();
    double a_res = (double)(a_end - a_start);
    time[2] += a_res;

    //timerv.add.end = clock();
    //timerv.add.total += (double)(timerv.add.end - timerv.add.start);
}

void sub(int d, int **a, int **b, int **result, double *time) {
    //Subtract two nxn 
    clock_t s_start = clock();

    int i,j;
    for (i = 0; i < d; i++) {
        for (j = 0; j < d; j++) {
            result[i][j] = a[i][j] - b[i][j];
        }
    }

    clock_t s_end = clock();
    double s_res = (double)(s_end - s_start);
    time[3] += s_res;

}

void mm_multi(int d, int **matrixm, int **matrixn, int **result) {
    //timerv.mult.start = clock();

    int i,j,k;

    /*for (i = 0; i < d; i++) {
        for (j = 0; j < d; j++) {
            result[i][j] = 0;
        }
    }*/

    for (i = 0; i < d; i++) {
        for (j = 0; j < d; j++) {
            for (k = 0; k < d; k++) {
                result[i][j] += matrixm[i][k] * matrixn[k][j];
            }
        }
    }

    //timerv.mult.end = clock();
    //timerv.mult.total += (double)(timerv.mult.end - timerv.mult.start);
}


void *p_str(void *threadarg) {
    int len, i, j, id;
    int **mm1, **mm2, **mm3, **mm4, **result, **hold, **hold2;
    double *time;
    struct MMDATA *my_data;
 
    //sleep(1);
    my_data = (struct MMDATA *) threadarg;
    id = my_data->id;
    len = my_data->len;
    mm1 = my_data->mm1;
    mm2 = my_data->mm2;
    mm3 = my_data->mm3;
    mm4 = my_data->mm4;
    result = my_data->m_result;
    time = my_data->time;

    hold = malloc(sizeof(int*) * len);
    hold2 = malloc(sizeof(int*) * len);

    for(i = 0; i < len; i++) {
        hold[i] = malloc(sizeof(int) * len);
        hold2[i] = malloc(sizeof(int) * len);
    }

    for(i = 0; i < len; i++) {
        for(j = 0; j < len; j++) {
            hold[i][j] = 0;
            hold2[i][j] = 0;
        }
    }
    
    if (id == 1) {
        add(len, mm1, mm2, hold, time);
        add(len, mm3, mm4, hold2, time);
        str_mm(len, len, hold, hold2, result, time);
    }
    else if (id == 2) {
        add(len, mm1, mm2, hold, time);
        str_mm(len, len, hold, mm3, result, time);
    }
    else if (id == 3) {
        sub(len, mm2, mm3, hold, time);
        str_mm(len, len, mm1, hold, result, time);
    }
    else if (id == 4) {
        sub(len, mm2, mm3, hold, time);
        str_mm(len, len, mm1, hold, result, time);
    }
    else if (id == 5) {
        add(len, mm1, mm2, hold, time);
        str_mm(len, len, hold, mm3, result, time);
    }
    else if (id == 6) {
        sub(len, mm1, mm2, hold, time);
        add(len, mm3, mm4, hold2, time);
        str_mm(len, len, hold, hold2, result, time);
    }
    else if (id == 7) {
        sub(len, mm1, mm2, hold, time);
        add(len, mm3, mm4, hold2, time);
        str_mm(len, len, hold, hold2, result, time);
    }
 
    for (i = 0; i < len; i++) {
        free(hold[i]);
        free(hold2[i]);
    }
    free(hold);
    free(hold2);

    //printf("Exiting thread #%d\n", id);
    pthread_exit((void*) 0);
}

void str_mm(int ht, int wt, int **matrixm, int **matrixn, int **result, double *time)
{
    if (wt <= thresh) {
        clock_t a_start = clock();
        mm_multi(wt, matrixm, matrixn, result);
        clock_t a_end = clock();
        double a_res = (double)(a_end - a_start);
        time[0] += a_res;
    }
    else {
        //timerv.split.start = clock();
        clock_t s_start = clock();

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

        int **m1, **m2, **m3, **m4, **m5, **m6, **m7, **hold;
        m1 = malloc(sizeof(int*) * len);
        m2 = malloc(sizeof(int*) * len);
        m3 = malloc(sizeof(int*) * len);
        m4 = malloc(sizeof(int*) * len);
        m5 = malloc(sizeof(int*) * len);
        m6 = malloc(sizeof(int*) * len);
        m7 = malloc(sizeof(int*) * len);
        hold = malloc(sizeof(int*) * len);
    
        double * pass_time1 = (double *)malloc(sizeof(double) * 4);
        double * pass_time2 = (double *)malloc(sizeof(double) * 4);
        double * pass_time3 = (double *)malloc(sizeof(double) * 4);
        double * pass_time4 = (double *)malloc(sizeof(double) * 4);
        double * pass_time5 = (double *)malloc(sizeof(double) * 4);
        double * pass_time6 = (double *)malloc(sizeof(double) * 4);
        double * pass_time7 = (double *)malloc(sizeof(double) * 4);

        int i, j;
        for(i = 0; i < len; i++) {
            A11[i] = malloc(sizeof(int) * len);
            A12[i] = malloc(sizeof(int) * len);
            A21[i] = malloc(sizeof(int) * len);
            A22[i] = malloc(sizeof(int) * len);
            
            B11[i] = malloc(sizeof(int) * len);
            B12[i] = malloc(sizeof(int) * len);
            B21[i] = malloc(sizeof(int) * len);
            B22[i] = malloc(sizeof(int) * len);

            m1[i] = malloc(sizeof(int) * len);
            m2[i] = malloc(sizeof(int) * len);
            m3[i] = malloc(sizeof(int) * len);
            m4[i] = malloc(sizeof(int) * len);
            m5[i] = malloc(sizeof(int) * len);
            m6[i] = malloc(sizeof(int) * len);
            m7[i] = malloc(sizeof(int) * len);
            hold[i] = malloc(sizeof(int) * len);

            C11[i] = malloc(sizeof(int) * len);
            C12[i] = malloc(sizeof(int) * len);
            C21[i] = malloc(sizeof(int) * len);
            C22[i] = malloc(sizeof(int) * len);
        }

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
            }
        }

        for (i = 0; i < 4; i++) {
            pass_time1[i] = 0;
            pass_time2[i] = 0;
            pass_time3[i] = 0;
            pass_time4[i] = 0;
            pass_time5[i] = 0;
            pass_time6[i] = 0;
            pass_time7[i] = 0;
        }

        clock_t s_end = clock();
        double s_res = (double)(s_end - s_start);
        time[1] += s_res;

        //timerv.split.end = clock();
        //timerv.split.total = (double)(timerv.split.end - timerv.split.start);

        int num_threads = 7;
        struct MMDATA multi_data[num_threads];
        pthread_t threads[num_threads];
        pthread_attr_t attr;
        void *status;
        int t;

        //Declare m1
        multi_data[0].len = len;
        multi_data[0].mm1 = A11;
        multi_data[0].mm2 = A22;
        multi_data[0].mm3 = B11;
        multi_data[0].mm4 = B22;
        multi_data[0].m_result = m1;
        multi_data[0].time = pass_time1;

        //Declare m2
        multi_data[1].len = len;
        multi_data[1].mm1 = A21;
        multi_data[1].mm2 = A22;
        multi_data[1].mm3 = B11;
        multi_data[1].mm4 = hold;
        multi_data[1].m_result = m2;
        multi_data[1].time = pass_time2;

        //Declare m3
        multi_data[2].len = len;
        multi_data[2].mm1 = A11;
        multi_data[2].mm2 = B12;
        multi_data[2].mm3 = B22;
        multi_data[2].mm4 = hold;
        multi_data[2].m_result = m3;
        multi_data[2].time = pass_time3;

        //Declare m4
        multi_data[3].len = len;
        multi_data[3].mm1 = A22;
        multi_data[3].mm2 = B21;
        multi_data[3].mm3 = B11;
        multi_data[3].mm4 = hold;
        multi_data[3].m_result = m4;
        multi_data[3].time = pass_time4;

        //Declare m5
        multi_data[4].len = len;
        multi_data[4].mm1 = A11;
        multi_data[4].mm2 = A12;
        multi_data[4].mm3 = B22;
        multi_data[4].mm4 = hold;
        multi_data[4].m_result = m5;
        multi_data[4].time = pass_time5;

        //Declare m6
        multi_data[5].len = len;
        multi_data[5].mm1 = A21;
        multi_data[5].mm2 = A11;
        multi_data[5].mm3 = B11;
        multi_data[5].mm4 = B12;
        multi_data[5].m_result = m6;
        multi_data[5].time = pass_time6;

        //Declare m7
        multi_data[6].len = len;
        multi_data[6].mm1 = A12;
        multi_data[6].mm2 = A22;
        multi_data[6].mm3 = B21;
        multi_data[6].mm4 = B22;
        multi_data[6].m_result = m7;
        multi_data[6].time = pass_time7;

        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        for(t=0; t<num_threads; t++) {
            //printf("Creating thread %d\n", t);
            multi_data[t].id = (t+1);
            pthread_create(&threads[t], &attr, p_str, (void *) &multi_data[t]);
        }

        pthread_attr_destroy(&attr);

        for(t=0; t<num_threads; t++) {
            pthread_join(threads[t], &status);
        }

        /*print_matrix(len, m1);
        print_matrix(len, m2);
        print_matrix(len, m3);
        print_matrix(len, m4);
        print_matrix(len, m5);
        print_matrix(len, m6);
        print_matrix(len, m7);*/

        for (i = 0; i < 4; i++) {
            time[i] += pass_time1[i];
            time[i] += pass_time2[i];
            time[i] += pass_time3[i];
            time[i] += pass_time4[i];
            time[i] += pass_time5[i];
            time[i] += pass_time6[i];
            time[i] += pass_time7[i];
        }

        //Solving C's
        add(len, m1, m4, C11, time);
        sub(len, C11, m5, C11, time);
        add(len, C11, m7, C11, time);
        
        add(len, m3, m5, C12, time);
        
        add(len, m2, m4, C21, time);
        
        sub(len, m1, m2, C22, time);
        add(len, C22, m3, C22, time);
        add(len, C22, m6, C22, time);

        for (i = 0; i < len; i++) {
            for (j = 0; j < len; j++) {
                result[i][j] = C11[i][j];
                result[i][j+len] = C12[i][j];
                result[i+len][j] = C21[i][j];
                result[i+len][j+len] = C22[i][j];
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

        free(C11);
        free(C12);
        free(C21);
        free(C22);

        free(pass_time1);
        free(pass_time2);
        free(pass_time3);
        free(pass_time4);
        free(pass_time5);
        free(pass_time6);
        free(pass_time7);
    }
}

int * strassen_mm(int *m, int *n, int wt, int ht, int t)
{
    //int m[] = {0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1};

    //int n[] = {1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0};

    //int wt = 16;
    //int ht = 16;

    //timerv.main.start = clock();
    clock_t m_start = clock();

    thresh = t;

    int **matrixm;
    matrixm = malloc(sizeof(int*) * ht);
    int **matrixn;
    matrixn = malloc(sizeof(int*) * ht);
    int **result;
    result = malloc(sizeof(int*) * ht);
    double * time = (double *)malloc(sizeof(double) * 4);
    int * answer = (int *)malloc(sizeof(int) * (ht * wt));
    
    int i,j;
    for(i = 0; i < ht; i++) {
        matrixm[i] = malloc(sizeof(int) * wt);
        matrixn[i] = malloc(sizeof(int) * wt);
        result[i] = malloc(sizeof(int) * wt);
    }

    //DELETE
    int count = 0;
    for (i = 0; i < ht; i++) {
        for (j = 0; j < wt; j++) {
            matrixm[i][j] = m[count];
            matrixn[i][j] = n[count];
            result[i][j] = 0;
            answer[count] = 0;
            count++;
        }
    }

    for (i = 0; i < 4; i++) {
        time[i] = 0;
    }


    str_mm(ht, wt, matrixm, matrixn, result, time);

    int hold = 0;
    for (i = 0; i < ht; i++) {
         for (j = 0; j < wt; j++) {
            answer[hold] = result[i][j];
            hold++;
        }
    }

    //timerv.main.end = clock();
    clock_t m_end = clock();
    //timerv.main.total = (double)(timerv.main.end - timerv.main.start);
    double main_time = (double)(m_end - m_start);

    //printf("TIME MAIN: %f\n", (double)timerv.main.total/CLOCKS_PER_SEC);
    printf("TIME MAIN VAR: %f\n", (double)main_time/CLOCKS_PER_SEC);
    printf("TIME NAIVE VAR: %f\n", (double)time[0]/CLOCKS_PER_SEC);
    printf("TIME SPLIT VAR: %f\n", (double)time[1]/CLOCKS_PER_SEC);
    printf("TIME ADD VAR: %f\n", (double)time[2]/CLOCKS_PER_SEC);
    printf("TIME SUB VAR: %f\n", (double)time[3]/CLOCKS_PER_SEC);
    //printf("TIME MULT/MAIN: %f\n", (double)timerv.mult.total/timerv.main.total);
    //printf("TIME ADD/MAIN: %f\n", (double)timerv.add.total/timerv.main.total);
    //printf("TIME SPLIT/MAIN: %f\n", (double)timerv.split.total/timerv.main.total);

    for (i = 0; i < ht; i++) {
        free(result[i]);
        free(matrixm[i]);
        free(matrixn[i]);
    }
    free(result);
    free(matrixm);
    free(matrixn);
    free(time);

    return answer;
}

