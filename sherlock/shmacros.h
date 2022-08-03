#ifndef SHMACROS_H
#define SHMACROS_H

#include <shmem.h>

#define YOU_SHALL_NOT_PASS() shmem_barrier_all()

#define SHMEQUAL SHMEM_CMP_EQ

// Actually maybe useful
int INT_MAX(int a, int b) { return (a > b) ? a : b; }

#define SHMAC_REDUCE_WORK_SIZE(_nreduce_) \
  INT_MAX(_nreduce_ / 2 + 1, SHMEM_REDUCE_MIN_WRKDATA_SIZE)

#endif /* SHMACROS_H */
