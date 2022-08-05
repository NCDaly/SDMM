// vandermonde_inverter.c

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "sdmm.h"
#include "mersenne_prime_field.h"
#include "vandermonde_inverter.h"

VdmInverter *vdm_inv_init(int size, int alpha_rng) {

  VdmInverter *inv = (VdmInverter *) malloc(sizeof(VdmInverter));
  inv->size = size;
  inv->prog = 0;
  inv->alpha_rng = alpha_rng;

  // Initialize all numerators, denominators, and alphas to 0
  inv->numer = (int *) calloc(size * size, sizeof(int)); 
  inv->denom = (int *) calloc(size, sizeof(int));
  inv->alpha = (int *) calloc(size, sizeof(int));

  // Set all denominators and the first row of numerators to 1
  for (int j = 0; j < size; j++) {
    inv->numer[j] = 1;
    inv->denom[j] = 1;
  }

  // Precompute field inverses for all alpha
  inv->alpha_inv = (int *) calloc(inv->alpha_rng, sizeof(int));
  for (int i = 1; i < inv->alpha_rng; i++) {
    inv->alpha_inv[i] = mpf_inv(i);
  }

  return inv;
}  

int vdm_inv_update(VdmInverter *inv, int new) {

  // Add a new column
  int k = inv->prog++;
  inv->alpha[k] = new;

  for (int j = 0; j < inv->size; j++) {
    // Skip the new column
    if (j == k) {
      continue;
    }

    // Update denominators for both columns
    // Note: denominator is stored as field inverse
    if (j < k) {
      if (inv->alpha[j] > inv->alpha[k]) {
	int diff = mpf_sub(inv->alpha[j], inv->alpha[k]);
	inv->denom[j] = mpf_mul(inv->denom[j], mpf_neg(inv->alpha_inv[diff]));
	inv->denom[k] = mpf_mul(inv->denom[k], inv->alpha_inv[diff]);
      } else {
	int diff = mpf_sub(inv->alpha[k], inv->alpha[j]);
	inv->denom[j] = mpf_mul(inv->denom[j], inv->alpha_inv[diff]);
	inv->denom[k] = mpf_mul(inv->denom[k], mpf_neg(inv->alpha_inv[diff]));
      }
    }
    
    // Update numerators from bottom to top
    for (int i = k + 1; i > 0; i--) {
      int this = i * inv->size + j; // Index of element (i, j)
      int next = this - inv->size; // Index of element (i, j - 1)
      inv->numer[this] = mpf_sub(inv->numer[this], inv->numer[next]);
      inv->numer[next] = mpf_mul(inv->numer[next], inv->alpha[k]);
    }
  }
  
  return inv->prog;
}

void vdm_inv_print(VdmInverter *inv) {

  printf("\nVandermonde Inverter (%d/%d)\n", inv->prog, inv->size);

  // Current alpha array
  printf("Alpha:");
  for (int i = 0; i < inv->prog; i++) {
    printf(" %d", inv->alpha[i]);
  }

  // Current inverse matrix
  printf("\nInverse:\n");
  if (inv->prog > 0) {
    printf(".-");
  }
  for (int j = 0; j < inv->prog; j++) {
    printf("----------");
    printf((j == inv->prog - 1) ? "-." : "---");
  }
  printf("\n");
  for (int i = 0; i < inv->size; i++) {
    // Numerators for this row
    printf((i < inv->prog) ? "| " : "  ");
    for (int j = 0; j < inv->size; j++) {
      int ij = i * inv->size + j;
      printf("%10d", inv->numer[ij]);
      printf((i < inv->prog && j == inv->prog - 1) ? " | " : "   ");
    }
    printf("\n");

    // Denominators (same between rows)
    printf((i < inv->prog) ? "| " : "  ");
    for (int j = 0; j < inv->size; j++) {
      int ij = i * inv->size + j;
      printf("%10d", inv->denom[j]);
      printf((i < inv->prog && j == inv->prog - 1) ? " | " : "   ");
    }
    printf("\n");

    // Horizontal rule
    if (i == inv->prog - 1) {
      printf("'-");
    } else if (i < inv->prog) {
      printf("| ");
    } else {
      printf("  ");
    }
    for (int j = 0; i < inv->prog && j < inv->prog; j++) {
      if (i == inv->prog - 1) {
	printf("----------");
	printf((j == inv->prog - 1) ? "-'" : "---");
      } else {
	printf("          ");
	printf((j == inv->prog - 1) ? " |" : "   ");
      }
    }
    printf("\n");
  }
}


TYPE vdm_inv_eval(VdmInverter *inv, TYPE *data, int i) {

  int sum = 0;
  int ij = i * inv->size;

  // Dot product of chosen row with data vector
  for (int j = 0; j < inv->size; j++) {
    // Remember, denominator has already been inverted!
    int inv_elem = mpf_mul(inv->numer[ij++], inv->denom[j]);
    int new_term = mpf_mul(inv_elem, (int) data[j]);
    sum = mpf_add(sum, new_term);
  }

  // Cast back to preprocessor-defined type
  return (TYPE) sum;
}

void vdm_inv_destroy(VdmInverter *inv) {

  free(inv->numer);
  free(inv->denom);
  free(inv->alpha);
  free(inv->alpha_inv);
  free(inv);
}

int print_equation(int *coeff, int len) {

  int terms_printed = 0;

  for (int i = 0; i < len; i++) {
    // Only print nonzero terms
    if (coeff[i] == 0) {
      continue;
    }
    // Sign
    if (coeff[i] < 0) {
      printf(" -");
    } else if (terms_printed > 0) {
      printf(" +");
    }
    // Coefficient
    printf(" ");
    if (i == 0 || coeff[i] != 1) {
      printf("%d", abs(coeff[i]));
    }
    // Variable
    if (i > 0) {
      printf("x");
    }
    // Exponenet
    if (i > 1) {
      printf("^%d", i);
    }

    terms_printed++;
  }

  return terms_printed;
}

/*
int main() {

  int size;
  printf("Number of equations (degree plus one): ");
  scanf("%d", &size);

  VdmInverter *inv = vdm_inv_init(size, 20);
  int *alpha = calloc(size, sizeof(int));
  int *coeff = calloc(size, sizeof(int));
  TYPE *data = calloc(size, sizeof(TYPE));

  vdm_inv_print(inv);

  // Build inverse step by step
  srand(time(0));
  for (int i = 0; i < size; i++) {
    printf("\nEnter alpha %d: ", i);
    scanf("%d", &alpha[i]);
    coeff[i] = rand() % MPF_VAL;
    vdm_inv_update(inv, alpha[i]);
    vdm_inv_print(inv);
  }
  
  // Print equation
  printf("Equation: f(x) =");
  print_equation(coeff, size);
  printf("\n");

  // Evaluate equations
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      int term = mpf_mul(coeff[j], mpf_pow(alpha[i], j));
      data[i] = (TYPE) mpf_add((int) data[i], term);
    }
  }

  // Compute and print interpolation
  for (int i = 0; i < size; i++) {
    coeff[i] = vdm_inv_eval(inv, data, i);
  }
  printf("Computed: f(x) =");
  print_equation(coeff, size);
  printf("\n");
  
  // Clean up
  vdm_inv_destroy(inv);
  free(alpha);
  free(coeff);
  free(data);
}
*/
