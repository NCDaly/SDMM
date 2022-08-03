#ifndef SDMM_H
#define SDMM_H

#include <stdlib.h>
#include <stdio.h>

// Uncomment one of these to set TYPE
#define TYPE1 int32_t
//#define TYPE2 int64_t

#ifdef TYPE1 /* int32_t */
#  define TYPE TYPE1
#  define shmem_T_p shmem_int32_p
#  define shmem_T_put shmem_int32_put
#elif defined TYPE2 /* int64_t */
#  define TYPE TYPE2
#  define shmem_T_p shmem_int64_p
#  define shmem_T_put shmem_int64_put
#endif

typedef struct {
  int rows;
  int cols;
  TYPE *data;
} Matrix;

typedef struct {
  int terms;
  int degree;
  Matrix *coeffs;
  int *exponents;
} Polynomial;

// Get a pointer to element (i, j) in row-major matrix
TYPE *element(Matrix *mat, int i, int j);

// Safely destroy a heap-allocated matrix
void destroy_matrix(Matrix *mat);

#endif /* SHMM_H */

