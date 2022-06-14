#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "function-stats.h"

/* Global Variable! */
Stats FUNC_STATS[STATS_LEN];

void setup_func(u_int8_t func_id, char *name) {

  Stats *func = &FUNC_STATS[func_id];

  func->name = name;
  func->state = 0;
  func->calls = 0;
  func->timer = 0;
}


void enter_func(u_int8_t func_id) {

  Stats *func = &FUNC_STATS[func_id];
  
  func->calls++;
  func->state++;
  if (func->state == 1) {
    func->timer -= clock();
  }
}

void leave_func(u_int8_t func_id) {

  Stats *func = &FUNC_STATS[func_id];

  func->state--;
  if (func->state == 0) {
    func->timer += clock();
  }
}

void print_stats(u_int8_t func_id) {

  Stats *func = &FUNC_STATS[func_id];
  Stats *main = &FUNC_STATS[FUNC_ID_MAIN];
  
  printf("%d %24s %12ld calls %12f seconds %3ld%%\n",
	 func_id, func->name, func->calls,
	 (double) func->timer / CLOCKS_PER_SEC,
	 100 * func->timer / main->timer);
}

void reset_stats(void) {

  for (u_int8_t func_id = 0; func_id < STATS_LEN; func_id++) {
    Stats *func = &FUNC_STATS[func_id];
    func->calls = 0;
    func->state = 0;
    func->timer = 0;
  }
}
