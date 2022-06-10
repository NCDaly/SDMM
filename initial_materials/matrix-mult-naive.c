#include <stdlib.h>
#include <stdio.h>

typedef struct {
  u_int16_t rows;
  u_int16_t cols;
  int32_t *data;
} Matrix;

int32_t *element(Matrix *mat, u_int16_t i, u_int16_t j) {

  if (i < 0 || i >= mat->rows) {
    return NULL;
  } else if (j < 0 || j >= mat->cols) {
    return NULL;
  } else {
    return (int32_t *) (mat->data + i * mat->cols + j);
  }
}

Matrix *multiply(Matrix *mat1, Matrix *mat2) {

  if (mat1->cols != mat2->rows) {
    return NULL;
  }
  
  Matrix *prod = malloc(sizeof(prod));
  prod->rows = mat1->rows;
  prod->cols = mat2->cols;
  prod->data = (int32_t *) calloc(prod->rows * prod->cols, sizeof(int32_t));

  for (u_int16_t i = 0; i < prod->rows; i++) {
    for (u_int16_t j = 0; j < prod->cols; j++) {
      for (u_int16_t k = 0; k < mat1->cols; k++) {
	*element(prod, i, j) += *element(mat1, i, k) * *element(mat2, k, j);
      }
    }
  }
  
  return prod;
}

Matrix *input_matrix() {

  Matrix *mat = malloc(sizeof(mat));
  
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
  Matrix *prod = multiply(mat1, mat2);

  print_matrix(prod);

  destroy_matrix(mat1);
  destroy_matrix(mat2);
  destroy_matrix(prod);

  return 0;
}
