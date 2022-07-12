// interpolate_matrix.c

#include <stdlib.h>
#include "sdmm.h"
#include "mersenne_prime_field.h"
#include "vandermonde_inverter.h"
#include "interpolate_matrix.h"

Matrix *interpolate_matrix_mat_dot(VdmInverter *inv, Matrix *shares, int term) {

  TYPE *data = (TYPE *) calloc(inv->size, sizeof(TYPE));
  int rows = shares->rows;
  int cols = shares->cols;

  Matrix *result = malloc(sizeof(Matrix));
  result->rows = rows;
  result->cols = cols;
  result->data = (TYPE *) calloc(rows * cols, sizeof(TYPE));

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      // Gather shares of element
      for (int k = 0; k < inv->size; k++) {
	int pe = inv->alpha[k];
	data[k] = *element(&shares[pe], i, j);
      }

      // Compute resulting element
      *element(result, i, j) = vdm_inv_eval(inv, data, term);
    }
  }

  free(data);
  return result;
}
