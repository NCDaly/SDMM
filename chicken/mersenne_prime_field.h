// mersenne_prime_field.h

#ifndef MERSENNE_PRIME_FIELD_H
#define MERSENNE_PRIME_FIELD_H

#include <stdlib.h>

#define MPF_EXP_0 2
#define MPF_EXP_1 3
#define MPF_EXP_2 5
#define MPF_EXP_3 7
#define MPF_EXP_4 13
#define MPF_EXP_5 17
#define MPF_EXP_6 19
#define MPF_EXP_7 31

#define MPF_EXP MPF_EXP_7
#define MPF_VAL ((1L << MPF_EXP) - 1)

// Pre: 0 <= a <= 4^MPF_EXP
int mpf_mod(long a);

// Pre: a is in the field
int mpf_neg(int a);
int mpf_inv(int a);

// Pre: a and b are in the field
int mpf_add(int a, int b);
int mpf_sub(int a, int b);
int mpf_mul(int a, int b);
int mpf_div(int a, int b);

#endif /* MERSENNE_PRIME_FIELD_H */
