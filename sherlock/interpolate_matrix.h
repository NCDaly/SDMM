// interpolate_matrix.h

#ifndef INTERPOLATE_MATRIX_H
#define INTERPOLATE_MATRIX_H

#include <stdlib.h>
#include "mersenne_prime_field.h"
#include "vandermonde_inverter.h"

Matrix *interpolate_matrix_mat_dot(VdmInverter *inv, Matrix *shares, int term);

#endif /* INTERPOLATE_MATRIX_H */

