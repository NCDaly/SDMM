// mersenne_prime_field.h

#ifndef MERSENNE_PRIME_FIELD_H
#define MERSENNE_PRIME_FIELD_H

#include <stdlib.h>
#include <stdio.h>

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
int mpf_mod(long a) {
 
  // First mod by 2^p
  long q0 = (a >> MPF_EXP); // = x / 2^p
  long r0 = (a & MPF_VAL); // = x % 2^p

  // Use this to mod by 2^p - 1
  long q = q0 + (q0 + r0 >= MPF_VAL);
  long r = (a + q) & MPF_VAL;

  return (int) r;
}

// Pre: a and b are in the field
int mpf_add(int a, int b) {

  return mpf_mod((long) a + (long) b);
}

// Pre: a is in the field
int mpf_neg(int a) {

  return mpf_mod(MPF_VAL - (long) a);
}

// Pre: a and b are in the field
int mpf_sub(int a, int b) {

  return mpf_mod((long) a + (long) mpf_neg(b));
}

// Pre: a and b are in the field
int mpf_mul(int a, int b) {

  return mpf_mod((long) a * (long) b);
}

// Pre: a and b are in the field
int mpf_inv(int a) {

  // Note that a^-1 = a^(2^p - 3) mod 2^p - 1
  // And, that 2^p - 1 = 1 + 2 + 4 + 8 + ... + 2^(p - 1)
  // Therefore 2^p - 3 = 1 + 0 + 4 + 8 + ... + 2^(p - 1)
  a = mpf_mod((long) a);
  int a_inv = a;
  a = mpf_mul(a, a);
  for (int i = 2; i < MPF_EXP; i++) {
    a = mpf_mul(a, a);
    a_inv = mpf_mul(a_inv, a);
  }
  return a_inv;
}

// Pre: a and b are in the field
int mpf_div(int a, int b) {
 
  return mpf_mod((long) a * (long) mpf_inv(b));
}

#endif /* MERSENNE_PRIME_FIELD_H */
