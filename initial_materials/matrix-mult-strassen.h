#ifndef MATRIX_MULT_STRASSEN_H
#define MATRIX_MULT_STRASSEN_H

#include <stdlib.h>

#define DEFAULT_THRESHOLD 128

typedef struct {
  u_int16_t rows;
  u_int16_t cols;
  int32_t *data;
} Matrix;

int32_t *element(Matrix *mat, u_int16_t i, u_int16_t j);

void decompose(Matrix *mat, Matrix subs[2][2]);
void compose(Matrix subs[2][2], Matrix *mat);

void compute_m1(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m1, u_int16_t threshold);
void compute_m2(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m2, u_int16_t threshold);
void compute_m3(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m3, u_int16_t threshold);
void compute_m4(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m4, u_int16_t threshold);
void compute_m5(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m5, u_int16_t threshold);
void compute_m6(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m6, u_int16_t threshold);
void compute_m7(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m7, u_int16_t threshold);

void compute_c11(Matrix m[7], Matrix *c11);
void compute_c12(Matrix m[7], Matrix *c12);
void compute_c21(Matrix m[7], Matrix *c21);
void compute_c22(Matrix m[7], Matrix *c22);

Matrix *add(Matrix *mat1, Matrix *mat2, Matrix *sum);
Matrix *subtract(Matrix *mat1, Matrix *mat2, Matrix *diff);
Matrix *multiply(Matrix *mat1, Matrix *mat2, Matrix *prod, u_int16_t threshold);
Matrix *naive_multiply(Matrix *mat1, Matrix *mat2, Matrix *prod);
Matrix *strassen_multiply(Matrix *mat1, Matrix *mat2, Matrix *prod, u_int16_t threshold);

Matrix *input_matrix();

void print_matrix(Matrix *mat);

void destroy_matrix(Matrix *mat);

#endif /* MATRIX_MULT_STRASSEN_H */
