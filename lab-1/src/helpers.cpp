#include "helpers.h"
#include <pthread.h>

prefix_sum_args_t* alloc_args(int n_threads) {
  return (prefix_sum_args_t*) malloc(n_threads * sizeof(prefix_sum_args_t));
}

int next_power_of_two(int x) {
    int pow = 1;
    while (pow < x) {
        pow *= 2;
    }
    return pow;
}


Barrier* alloc_barrier(bool spin, int n_threads) {
    // return ( static_cast<Barrier*>(new PthreadBarrier(n_threads)));
    return (spin ? static_cast<Barrier*>(new SemaphoreBarrier(n_threads)) : 
            static_cast<Barrier*>(new PthreadBarrier(n_threads)));
}


// Create an args worker packet for each thread
void fill_args(prefix_sum_args_t *args,
               Barrier *basic_barrier,
               // pthread_barrier_t *basic_barrier,
               int n_threads,
               int n_vals,
               int *inputs,
               int *outputs,
               bool spin,
               int (*op)(int, int, int),
               int n_loops,
               int std_chunk_size,
               int prev_max,
               int chunk_n_vals
               ) {

    int leftovers = 0;

    if (leftovers > 1){
        leftovers = n_vals % ( (n_threads - 1) * std_chunk_size);
    }             

    for (int i = 0; i < n_threads; ++i) {
        printf("\n");
        printf("\nFilling thread %d args", i);
        args[i].input_vals = &inputs[i*std_chunk_size];
        args[i].output_vals = &outputs[i*std_chunk_size];
        args[i].spin = spin;
        args[i].n_vals = n_vals;
        args[i].n_threads = n_threads;
        args[i].t_id = i;
        args[i].op = op;
        args[i].n_loops = n_loops;
        args[i].std_chunk_size = std_chunk_size;
        args[i].basic_barrier = basic_barrier;
        // args[i].barrier = basic_barrier;

//        args[i].prev_max = &prev_max;

        // Re-assign reduced num of vals to each chunk
        // Case 1: All except last chunk
        if(i < n_threads-1) {
            args[i].chunk_n_vals = std_chunk_size;
            printf("\nFilled std thread %d chunk n_vals: %d", i, n_vals/n_threads);

            args[i+1].prev_max = &outputs[(i+1)*std_chunk_size-1];
//            printf("\nFilled thread %d prev_max:", i+1);
//            printf(args[i+1].prev_max);

        }
        // Case 2: last chunk that might have more num of vals
        else {
            if (leftovers) {
                args[i].chunk_n_vals = leftovers;
                printf("\n");
                printf("\nFilled last thread %d chunk n_vals: %d", i, leftovers);
            }
            else {
                args[i].chunk_n_vals = std_chunk_size;
                printf("\n");
                printf("\nFilled last thread %d chunk n_vals: %d", i, n_vals/n_threads);

            }
        }
    }
}


