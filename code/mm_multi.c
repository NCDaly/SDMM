#include <stdio.h>
#include <malloc.h>


int * mm_multi(int *m, int *n, int wt_m, int ht_m, int wt_n, int ht_n)
{
    //int m[] = {2,1,4,0,1,1};
    //int n[] = {6,3,-1,0,1,1,0,4,-2,5,0,2};
    //int wt_m = 3;
    //int ht_m = 2;
    //int wt_n = 4;
    //int ht_n = 3;    

    int matrixm[ht_m][wt_m];
    int matrixn[ht_n][wt_n];
    int result[ht_m][wt_n];
    int * answer = (int *)malloc(sizeof(int) * (ht_m * wt_n));
    
    int count = 0;
    for (int i = 0; i <ht_m; i++) {
        for (int j = 0; j < wt_m; j++) {
            matrixm[i][j] = m[count];
            count++;
        }
    }

    int count2 = 0;
    for (int k = 0; k <ht_n; k++) {
        for (int l = 0; l < wt_n; l++) {
            matrixn[k][l] = n[count2];
            count2++;
        }
    }

    // Initializing elements of matrix mult to 0.
    for (int i = 0; i < ht_m; i++) {
        for (int j = 0; j < wt_n; j++) {
            result[i][j] = 0;
        }
    }

    // Multiplying first and second matrices and storing it in result
    for (int i = 0; i < ht_m; i++) {
        for (int j = 0; j < wt_n; j++) {
            for (int k = 0; k < wt_m; ++k) {
                result[i][j] += matrixm[i][k] * matrixn[k][j];
            }
        }
    }

    int hold = 0;
    for (int i = 0; i < ht_m; i++) {
        for (int j = 0; j < wt_n; j++) {
            answer[hold] = result[i][j];
            hold++;
        }
    }

    return answer;
}
