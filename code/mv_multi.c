#include <stdio.h>
#include <malloc.h>

int * mv_multi(int *vector, int *matrix, int wt, int ht, int len)
{
    int * answer = (int *)malloc(sizeof(int) * ht);
    answer[0] = 0;
    answer[1] = 0;
    answer[2] = 0;
    answer[3] = 0;

    int i;
    int check = -1;
    // go through matrix
    for (i=0; i<len; i++)
    {
        int hold = i % wt;
        if (hold == 0) {
            check++;
        }

        answer[check] += matrix[i] * vector[hold];        
    }

    //answer[0] = matrix[0];
    //answer[1] = (0 % wt);
    //answer[2] = matrix[0] * vector[0];
    //answer[3] = 0;
    
    return answer;
    //return 0;
}
