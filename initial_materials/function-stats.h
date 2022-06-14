#ifndef FUNCTION_STATS_H
#define FUNCTION_STATS_H

#include <stdlib.h>
#include <time.h>

#ifndef STATS_LEN
#define STATS_LEN 16
#endif

#define FLAG_STARTED 1

#define FUNC_ID_MAIN 0

typedef struct {
  char *name;
  int32_t state;
  int64_t calls;
  clock_t timer;
} Stats;

void setup_func(u_int8_t func_id, char *name);
void enter_func(u_int8_t func_id);
void leave_func(u_int8_t func_id);
void print_stats(u_int8_t func_id);
void reset_stats();

#endif /* FUNCTION_STATS_H */
