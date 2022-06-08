#include <stdlib.h>
#include <stdio.h>

typedef struct {
  int rows;
  int cols;
  int *data;
} Matrix;

int *element(Matrix mat, int i, int j) {

  if (i < 0 || i >= mat.rows) {
    return NULL;
  } else if (j < 0 || j >= mat.cols) {
    return NULL;
  } else {
    return (int *) (mat.data + i * mat.cols + j);
  }
}

Matrix multiply(Matrix mat1, Matrix mat2) {

  Matrix prod;
  
  if (mat1.cols != mat2.rows) {
    prod.rows = 0;
    prod.cols = 0;
    prod.data = NULL;
  } else {
    prod.rows = mat1.rows;
    prod.cols = mat2.cols;
    prod.data = (int *) calloc(prod.rows * prod.cols, sizeof(int));
  }

  for (int i = 0; i < prod.rows; i++) {
    for (int j = 0; j < prod.cols; j++) {
      for (int k = 0; k < mat1.cols; k++) {
	*element(prod, i, j) += *element(mat1, i, k) * *element(mat2, k, j);
      }
    }
  }
  
  return prod;
}

Matrix input_matrix() {

  Matrix mat;
  
  printf("# rows: ");
  scanf("%d", &mat.rows);
  printf("# cols: ");
  scanf("%d", &mat.cols);
  
  mat.data = (int *) calloc(mat.rows * mat.cols, sizeof(int));
  printf("Enter matrix:\n");
  for (int i = 0; i < mat.rows; i++) {
    for (int j = 0; j < mat.cols; j++) {
      scanf("%d", element(mat, i, j));
    }
  }
  
  return mat;
}

void print_matrix(Matrix mat) {

  for (int i = 0; i < mat.rows; i++) {
    for (int j = 0; j < mat.cols; j++) {
      printf("%4d", *element(mat, i, j));
    }
    printf("\n");
  }
}

int main(int argc, char **argv) {

  Matrix mat1, mat2, prod;
  printf("Matrix A:\n");
  mat1 = input_matrix();
  printf("Matrix B:\n");
  mat2 = input_matrix();
  printf("Product AB:\n");
  prod = multiply(mat1, mat2);
  print_matrix(prod);
  return 0;
}
