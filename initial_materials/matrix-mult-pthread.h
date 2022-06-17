#ifndef MATRIX_MULT_PTHREAD_H
#define MATRIX_MULT_PTHREAD_H

#include <pthread.h>
#include <stdlib.h>

#define DEFAULT_THRESHOLD 128
#define DEFAULT_RECURSION 1
#define MAX_THREADS 2401

#define TASK_ID_M1 0
#define TASK_ID_M2 1
#define TASK_ID_M3 2
#define TASK_ID_M4 3
#define TASK_ID_M5 4
#define TASK_ID_M6 5
#define TASK_ID_M7 6
#define TASK_ID_C11 7
#define TASK_ID_C12 8
#define TASK_ID_C21 9
#define TASK_ID_C22 10

#define _11 0
#define _12 1
#define _21 2
#define _22 3

typedef struct {
  u_int16_t rows;
  u_int16_t cols;
  int32_t *data;
} Matrix;

typedef struct {
  u_int16_t size;
  u_int8_t id;
  Matrix *a;
  Matrix *b;
  Matrix *c;
  Matrix *m;
} Task;

void create_pthread_tools(void);
void destroy_pthread_tools(void);

void set_strassen_threshold(u_int16_t value);
void set_parallel_recursion(u_int16_t value);

void *perform_task(void *task_ptr);
void assign_task(u_int8_t task_id, Task *task_info, pthread_t *thread);

int32_t *element(Matrix *mat, u_int16_t i, u_int16_t j);

void decompose(Matrix *mat, Matrix subs[2][2]);
void compose(Matrix subs[2][2], Matrix *mat);

void compute_m1(Matrix *sub1, Matrix *sub2, Matrix *m1);
void compute_m2(Matrix *sub1, Matrix *sub2, Matrix *m2);
void compute_m3(Matrix *sub1, Matrix *sub2, Matrix *m3);
void compute_m4(Matrix *sub1, Matrix *sub2, Matrix *m4);
void compute_m5(Matrix *sub1, Matrix *sub2, Matrix *m5);
void compute_m6(Matrix *sub1, Matrix *sub2, Matrix *m6);
void compute_m7(Matrix *sub1, Matrix *sub2, Matrix *m7);

void compute_c11(Matrix *m, Matrix *c11);
void compute_c12(Matrix *m, Matrix *c12);
void compute_c21(Matrix *m, Matrix *c21);
void compute_c22(Matrix *m, Matrix *c22);

Matrix *add(Matrix *mat1, Matrix *mat2, Matrix *sum);
Matrix *subtract(Matrix *mat1, Matrix *mat2, Matrix *diff);
Matrix *multiply(Matrix *mat1, Matrix *mat2, Matrix *prod);
Matrix *naive_multiply(Matrix *mat1, Matrix *mat2, Matrix *prod);
Matrix *strassen_multiply(Matrix *mat1, Matrix *mat2, Matrix *prod);

Matrix *input_matrix();

void print_matrix(Matrix *mat);

void destroy_matrix(Matrix *mat);

#endif /* MATRIX_MULT_PTHREAD_H */
