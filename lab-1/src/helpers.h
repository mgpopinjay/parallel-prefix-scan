#pragma once

#include "operators.h"
#include <stdlib.h>
#include <pthread.h>
#include <spin_barrier.h>

struct prefix_sum_args_t {
  int*               input_vals;
  int*               output_vals;
  bool               spin;
  int                n_vals;
  int                n_threads;
  int                t_id;
  int                (*op)(int, int, int);  // Onces pointed as a function, it can be invoked directly.
  int                n_loops;
  int                std_chunk_size;
  int                *prev_max;
  int                chunk_n_vals;
  // void              *barrier;
  pthread_barrier_t  *barrier;
};

// Direct allocated data memory to data struct pointer
prefix_sum_args_t* alloc_args(int n_threads);

int next_power_of_two(int x);

void fill_args(prefix_sum_args_t *args,
               pthread_barrier_t *basic_barrier,
               int n_threads,
               int n_vals,
               int *inputs,
               int *outputs,
               bool spin,
               int (*op)(int, int, int),
               int n_loops,
               int std_chunk_size,
               int prev_max,
               int chunk_n_vals)
               ;
