#include <stdio.h>
#include <malloc.h>


//int * mm_multi(int *m, int *n, int wt_m, int ht_m, int wt_n, int ht_n)
int main()
{
    int m[] = {1,2,3,4};
    int n[] = {5,6,7,8};
    int wt_m = 2;
    int ht_m = 2;
    int wt_n = 2;
    int ht_n = 2;    

    int matrixm[ht_m][wt_m];
    int matrixn[ht_n][wt_n];
    //int result[ht_m][wt_n];
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

    /*printf("\nOutput Matrix:\n");
    for (int i = 0; i < ht_m; i++) {
        for (int j = 0; j < wt_m; j++) {
            printf("%d  ", matrixm[i][j]);
        }
    }*/


    /*// Initializing elements of matrix mult to 0.
    for (int i = 0; i < ht_m; i++) {
        for (int j = 0; j < wt_n; j++) {
            result[i][j] = 0;
        }
    }*/

    int m1 = (matrixm[0][0] + matrixm[1][1]) * (matrixn[0][0] + matrixn[1][1]);
    int m2 = (matrixm[1][0] + matrixm[1][1]) * matrixn[0][0];
    int m3 = matrixm[0][0] * (matrixn[0][1] - matrixn[1][1]);
    int m4 = matrixm[1][1] * (matrixn[1][0] - matrixn[0][0]);
    int m5 = (matrixm[0][0] + matrixm[0][1]) * matrixn[1][1];
    int m6 = (matrixm[1][0] - matrixm[0][0]) * (matrixn[0][0] + matrixn[0][1]);
    int m7 = (matrixm[0][1] - matrixm[1][1]) * (matrixn[1][0] + matrixn[1][1]);

    answer[0] = m1 + m4 - m5 + m7;
    answer[1] = m3 + m5;
    answer[2] = m2 + m4;
    answer[3] = m1 - m2 + m3 + m6;

    /*for (i = 0; i < r; ++i) {
        for (j = 0; j < c; ++j) {
            sum[i][j] = a[i][j] + b[i][j];
        }
    }*/

    // Multiplying first and second matrices and storing it in result
    /*for (int i = 0; i < ht_m; i++) {
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
    }*/

    printf("\nOutput Matrix:\n");
    for (int i = 0; i < 4; i++) {
        printf("%d  ", answer[i]);
    }

    //return answer;
    return 0;
}
