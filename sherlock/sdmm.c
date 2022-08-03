// sdmm.c

#include <stdlib.h>
#include "sdmm.h"

// Get a pointer to element (i, j) in row-major matrix
TYPE *element(Matrix *mat, int i, int j) {

  return (TYPE *) (mat->data + i * mat->cols + j);
}

// Safely destroy a heap-allocated matrix
void destroy_matrix(Matrix *mat) {

  free(mat->data);
  free(mat);
}
