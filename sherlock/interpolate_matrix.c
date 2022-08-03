// interpolate_matrix.c

#include <stdlib.h>
#include "sdmm.h"
#include "mersenne_prime_field.h"
#include "vandermonde_inverter.h"
#include "interpolate_matrix.h"

Matrix *interpolate_matrix_mat_dot(VdmInverter *inv, Matrix *shares, int term) {

  //TYPE *data = (TYPE *) calloc(inv->size, sizeof(TYPE));
  int first_done = inv->alpha[0];
  int rows = shares[first_done].rows;
  int cols = shares[first_done].cols;

  Matrix *result = malloc(sizeof(Matrix));
  result->rows = rows;
  result->cols = cols;
  result->data = (TYPE *) calloc(rows * cols, sizeof(TYPE));

  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      /*
      // Gather shares of element
      for (int k = 0; k < inv->size; k++) {
        int pe = inv->alpha[k];
        data[k] = *element(&shares[pe], i, j);
      }

      //Compute resulting element
      *element(result, i, j) = vdm_inv_eval(inv, data, term);
      */
      int offset = term * inv->size;
      for (int k = 0; k < inv->size; k++) {
        int pe = inv->alpha[k];
	int coeff = inv->numer[offset + k];
	int new_term = mpf_mul(coeff, (int) *element(&shares[pe], i, j));
	*element(result, i, j) = mpf_add(*element(result, i, j), new_term);
      }
    }
  }
  
  //free(data);
  return result;
}