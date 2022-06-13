#include <stdlib.h>
#include <stdio.h>

#include "matrix-mult-strassen.h"

int32_t *element(Matrix *mat, u_int16_t i, u_int16_t j) {

  if (i < 0 || i >= mat->rows) {
    return NULL;
  } else if (j < 0 || j >= mat->cols) {
    return NULL;
  } else {
    return (int32_t *) (mat->data + i * mat->cols + j);
  }
}

void decompose(Matrix *mat, Matrix subs[2][2]) {

  // Assume the matrix is square
  
  // Initialize the new matrices
  u_int16_t size = mat->rows / 2;
  for (u_int8_t i = 0; i < 2; i++) {
    for (u_int8_t j = 0; j < 2; j++) {
      subs[i][j].rows = size;
      subs[i][j].cols = size;
      subs[i][j].data = (int32_t *) calloc(size * size, sizeof(int32_t));
    }
  }

  // Fill the new matrices
  for (u_int16_t i = 0; i < mat->rows; i++) {
    for (u_int16_t j = 0; j < mat->cols; j++) {
      *element(&subs[i / size][j / size], i % size, j % size) = *element(mat, i, j);
    }
  }
}

void compose(Matrix subs[2][2], Matrix *mat) {

  // Assume the matrix is square
  
  // Initialize the new matrix
  u_int16_t size = subs[0][0].rows;
  mat->rows = size * 2;
  mat->cols = size * 2;
  mat->data = (int32_t *) calloc(mat->rows * mat->cols, sizeof(int32_t));

  // Fill the new matrix
  for (u_int16_t i = 0; i < mat->rows; i++) {
    for (u_int16_t j = 0; j < mat->rows; j++) {
      *element(mat, i, j) = *element(&subs[i / size][j / size], i % size, j % size);
    }
  }
}

void compute_m1(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m1, u_int16_t threshold) {

  Matrix temp1;
  Matrix temp2;

  add(&sub1[0][0], &sub1[1][1], &temp1);
  add(&sub2[0][0], &sub2[1][1], &temp2);
  multiply(&temp1, &temp2, m1, threshold);

  free(temp1.data);
  free(temp2.data);
}

void compute_m2(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m2, u_int16_t threshold) {

  Matrix temp;

  add(&sub1[1][0], &sub1[1][1], &temp);
  multiply(&temp, &sub2[0][0], m2, threshold);

  free(temp.data);
}

void compute_m3(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m3, u_int16_t threshold) {

  Matrix temp;

  subtract(&sub2[0][1], &sub2[1][1], &temp);
  multiply(&sub1[0][0], &temp, m3, threshold);

  free(temp.data);
}

void compute_m4(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m4, u_int16_t threshold) {

  Matrix temp;

  subtract(&sub2[1][0], &sub2[0][0], &temp);
  multiply(&sub1[1][1], &temp, m4, threshold);

  free(temp.data);
}

void compute_m5(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m5, u_int16_t threshold) {

  Matrix temp;

  add(&sub1[0][0], &sub1[0][1], &temp);
  multiply(&temp, &sub2[1][1], m5, threshold);

  free(temp.data);
}

void compute_m6(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m6, u_int16_t threshold) {

  Matrix temp1;
  Matrix temp2;

  subtract(&sub1[1][0], &sub1[0][0], &temp1);
  add(&sub2[0][0], &sub2[0][1], &temp2);
  multiply(&temp1, &temp2, m6, threshold);

  free(temp1.data);
  free(temp2.data);
}

void compute_m7(Matrix sub1[2][2], Matrix sub2[2][2], Matrix *m7, u_int16_t threshold) {

  Matrix temp1;
  Matrix temp2;

  subtract(&sub1[0][1], &sub1[1][1], &temp1);
  add(&sub2[1][0], &sub2[1][1], &temp2);
  multiply(&temp1, &temp2, m7, threshold);

  free(temp1.data);
  free(temp2.data);
}

void compute_c11(Matrix m[7], Matrix *c11) {

  Matrix temp1;
  Matrix temp2;

  subtract(&m[0], &m[4], &temp1);
  add(&m[3], &m[6], &temp2);
  add(&temp1, &temp2, c11);

  free(temp1.data);
  free(temp2.data);
}

void compute_c12(Matrix m[7], Matrix *c12) {

  add(&m[2], &m[4], c12);
}

void compute_c21(Matrix m[7], Matrix *c21) {

  add(&m[1], &m[3], c21);
}

void compute_c22(Matrix m[7], Matrix *c22) {

  Matrix temp1;
  Matrix temp2;

  subtract(&m[0], &m[1], &temp1);
  add(&m[2], &m[5], &temp2);
  add(&temp1, &temp2, c22);

  free(temp1.data);
  free(temp2.data);
}

Matrix *add(Matrix *mat1, Matrix *mat2, Matrix *sum) {

  // Check that both matrices have the same dimensions
  if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
    return NULL;
  }

  // Initialize the new matrix
  sum->rows = mat1->rows;
  sum->cols = mat2->cols;
  sum->data = (int32_t *) calloc(sum->rows * sum->cols, sizeof(int32_t));
  if (sum->data == NULL) {
    return NULL;
  }
  
  // Compute the sum
  for (u_int16_t i = 0; i < sum->rows; i++) {
    for (u_int16_t j = 0; j < sum->cols; j++) {
      *element(sum, i, j) = *element(mat1, i, j) + *element(mat2, i, j);
    }
  }

  return sum;
}

Matrix *subtract(Matrix *mat1, Matrix *mat2, Matrix *diff) {

  // Check that both matrices have the same dimensions
  if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
    return NULL;
  }

  // Initialize the new matrix
  diff->rows = mat1->rows;
  diff->cols = mat2->cols;
  diff->data = (int32_t *) calloc(diff->rows * diff->cols, sizeof(int32_t));

  // Compute the sum
  for (u_int16_t i = 0; i < diff->rows; i++) {
    for (u_int16_t j = 0; j < diff->cols; j++) {
      *element(diff, i, j) = *element(mat1, i, j) - *element(mat2, i, j);
    }
  }

  return diff;
}

Matrix *multiply(Matrix *mat1, Matrix *mat2, Matrix *prod, u_int16_t threshold) {

  // Check that the matrices have the same inner dimension
  if (mat1->cols != mat2->rows) {
    return NULL;
  }

  // Initialize the new matrix
  prod->rows = mat1->rows;
  prod->cols = mat2->cols;

  // Perform the appropriate multiplication algorithm
  if (mat1->cols < threshold) {
    return naive_multiply(mat1, mat2, prod);
  } else {
    return strassen_multiply(mat1, mat2, prod, threshold);
  }
}

Matrix *naive_multiply(Matrix *mat1, Matrix *mat2, Matrix *prod) {

  // Assume the matrices have the same inner dimension

  // Build the matrix
  prod->data = (int32_t *) calloc(prod->rows * prod->cols, sizeof(int32_t));
  
  // Compute the product
  for (u_int16_t i = 0; i < prod->rows; i++) {
    for (u_int16_t j = 0; j < prod->cols; j++) {
      for (u_int16_t k = 0; k < mat1->cols; k++) {
	*element(prod, i, j) += *element(mat1, i, k) * *element(mat2, k, j);
      }
    }
  }
  
  return prod;
}

Matrix *strassen_multiply(Matrix *mat1, Matrix *mat2, Matrix *prod, u_int16_t threshold) {

  // Assume the matrices have the same inner dimension

  // Check that the matrices have even dimensions
  if (mat1->rows & 1 || mat1->cols & 1 || mat2->rows & 1 || mat2->cols & 1) {
    return NULL; // TODO: fix this rather than quitting
  }

  // Check that the matrices are square
  if (mat1->rows != mat1->cols || mat2->rows != mat2->cols) {
    return NULL; // TODO: fix this rather than quitting
  }

  // Split the matrices
  Matrix sub1[2][2];
  Matrix sub2[2][2];
  decompose(mat1, sub1);
  decompose(mat2, sub2);

  // Compute temporary matrices (M1, ..., M7)
  Matrix temp[7];
  compute_m1(sub1, sub2, &temp[0], threshold);
  compute_m2(sub1, sub2, &temp[1], threshold);
  compute_m3(sub1, sub2, &temp[2], threshold);
  compute_m4(sub1, sub2, &temp[3], threshold);
  compute_m5(sub1, sub2, &temp[4], threshold);
  compute_m6(sub1, sub2, &temp[5], threshold);
  compute_m7(sub1, sub2, &temp[6], threshold);
  
  // Compute submatrices (C11, C12, C21, C22) and create product
  Matrix sub3[2][2];
  compute_c11(temp, &sub3[0][0]);
  compute_c12(temp, &sub3[0][1]);
  compute_c21(temp, &sub3[1][0]);
  compute_c22(temp, &sub3[1][1]);
  compose(sub3, prod);

  // Clean up
  for (u_int8_t i = 0; i < 2; i++) {
    for (u_int8_t j = 0; j < 2; j++) {
      free(sub1[i][j].data);
      free(sub2[i][j].data);
      free(sub3[i][j].data);
    }
  }

  for (u_int8_t i = 0; i < 7; i++) {
    free(temp[i].data);
  }
  
  return prod;
}

Matrix *input_matrix() {

  Matrix *mat = malloc(sizeof(Matrix));
  
  printf("# rows: ");
  scanf("%hu", &mat->rows);
  printf("# cols: ");
  scanf("%hu", &mat->cols);
  
  mat->data = (int32_t *) calloc(mat->rows * mat->cols, sizeof(int32_t));
  printf("Enter matrix:\n");
  for (u_int16_t i = 0; i < mat->rows; i++) {
    for (u_int16_t j = 0; j < mat->cols; j++) {
      scanf("%d", element(mat, i, j));
    }
  }
  
  return mat;
}

void print_matrix(Matrix *mat) {

  for (u_int16_t i = 0; i < mat->rows; i++) {
    for (u_int16_t j = 0; j < mat->cols; j++) {
      printf("%4d", *element(mat, i, j));
    }
    printf("\n");
  }
}

void destroy_matrix(Matrix *mat) {

  free(mat->data);
  free(mat);
}

int main(int argc, char **argv) {

  printf("Matrix A:\n");
  Matrix *mat1 = input_matrix();

  printf("Matrix B:\n");
  Matrix *mat2 = input_matrix();

  printf("Product AB:\n");
  Matrix *prod = (Matrix *) malloc(sizeof(Matrix));
  multiply(mat1, mat2, prod, DEFAULT_THRESHOLD);

  //print_matrix(prod);

  destroy_matrix(mat1);
  destroy_matrix(mat2);
  destroy_matrix(prod);

  return 0;
}



