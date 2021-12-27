#ifndef _THREADS_H
#define _THREADS_H

#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <prefix_sum.h>
#include "helpers.h"

pthread_t* alloc_threads(int n_threads);

void start_threads(pthread_t*               threads,
                  int                       n_threads,
                  struct prefix_sum_args_t* args,
                  void* (*start_routine) (void*));

void join_threads(pthread_t* threads,
                  int        n_threads);

#endif
