#include "helpers.h"

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

// Create an args worker packet for each thread
void fill_args(prefix_sum_args_t *args,
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

//    int leftovers = n_vals - (n_threads * std_chunk_size);
    int leftovers = n_vals % ( (n_threads - 1) * std_chunk_size);
//    int final_chunk_size = std_chunk_size + leftovers;

    // Fill args for sequential case
    //            args[i] = {inputs, outputs, spin, n_vals,
//                       n_threads, i, op, n_loops};


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
//        args[i].prev_max = &prev_max;

        // Re-assign reduced num of vals to each chunk
        // Case 1: All except last chunk
        if(i < n_threads-1) {
            args[i].chunk_n_vals = std_chunk_size;
//            args[i].n_vals = n_vals/n_threads;
            printf("\nFilled std thread %d chunk n_vals: %d", i, n_vals/n_threads);

            args[i+1].prev_max = &outputs[(i+1)*std_chunk_size-1];
//            printf("\nFilled thread %d prev_max:", i+1);
//            printf(args[i+1].prev_max);

        }
        // Case 2: last chunk that might have more num of vals
        else {
//            args[i].chunk_n_vals = final_chunk_size;
            if (leftovers) {
//                args[i].n_vals = leftovers;
                args[i].chunk_n_vals = leftovers;
                printf("\n");
                printf("\nFilled last thread %d chunk n_vals: %d", i, leftovers);
            }
            else {
//                args[i].n_vals = n_vals/n_threads;
                args[i].chunk_n_vals = std_chunk_size;
                printf("\n");
                printf("\nFilled last thread %d chunk n_vals: %d", i, n_vals/n_threads);

            }
        }
    }
}


