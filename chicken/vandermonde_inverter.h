// vandermonde_inverter.h

#ifndef VANDERMONDE_INVERTER_H
#define VANDERMONDE_INVERTER_H

#include <stdlib.h>
#include "sdmm.h"

typedef struct {
  int size;
  int prog;
  int alpha_rng;
  int *numer;
  int *denom;
  int *alpha;
  int *alpha_inv;
} VdmInverter;

VdmInverter *vdm_inv_init(int size, int alpha_rng);

int vdm_inv_update(VdmInverter *inv, int new);

void vdm_inv_print(VdmInverter *inv);

TYPE vdm_inv_eval(VdmInverter *inv, TYPE *data, int i);

void vdm_inv_destroy(VdmInverter *inv);

int print_equation(int *coeff, int len);

#endif /* VANDERMONDE_INVERTER_H */
