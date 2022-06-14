#include <stdio.h>
#include <malloc.h>
 
int main()
{
    int vector[3] = {1,-1,-1};
    int matrix[12] = {1,-1,-1,1,-1,1,1,1,-1,-1,-1,-1};
    int wt = 3;
    int ht = 4;
    int len = 12;

    int * answer = (int *)malloc(sizeof(int) * ht);

    int i, k, j;
    int check = -1;
    // go through matrix
    for (i=0; i<len; i++)
    {
        int hold = i % wt;
        if (hold == 0) {
            check++;
        }
        printf("hold: %d\n", hold);
        printf("hold2: %d\n", check);
        answer[check] += matrix[i] * vector[hold];

        printf("matrix: %d\n", matrix[i]);
        printf("vector: %d\n", vector[hold]);
        printf("ans: %d\n", answer[check]); 

        //printf("ans: %d\n", answer[hold]);
    }
 
    for (k=0; k<ht; k++)
    {
        printf("%d\n", answer[k]);
    }
 
    //return answer;
    return 0;
}

