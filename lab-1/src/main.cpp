#include <stdio.h>
#include <iostream>
#include <argparse.h>
#include <threads.h>
#include <io.h>
#include <chrono>
#include <cstring>
#include "operators.h"
#include "helpers.h"
#include "prefix_sum.h"
#include <pthread.h>
#include <math.h>


using namespace std;

//// These three global variables are used in the implementation of a simple counter barrier
//pthread_cond_t end_of_phase;   // condition variable - waiting for all threads to reach barrier
//pthread_mutex_t phase_barrier; // mutex lock - to ensure atomicity of barrier function
//int number_complete_phase;     // count variable - the number of threads currently at the barrier



// Initialize standard pthread barrier
pthread_barrier_t basic_barrier;



int main(int argc, char **argv)
{
 
    printf("\nInside main()");

    // Parse args
    struct options_t opts;
    get_opts(argc, argv, &opts);

    bool sequential = false;
    if (opts.n_threads == 0) {
        opts.n_threads = 1;
        sequential = true;
    }

    printf("\nIn main()");
    printf("\nNum of threads: %d \n", opts.n_threads);


    // pthread_barrier_t basic_barrier;


    pthread_barrier_init(&basic_barrier, NULL, opts.n_threads);


//    // Initialize the barrier mutex and condition variable
//    pthread_mutex_init(&phase_barrier, NULL);
//    pthread_cond_init(&end_of_phase, NULL);
//    number_complete_phase = 0;


    // Setup threads: allocates memory for worker threads
    pthread_t *threads = sequential ? NULL : alloc_threads(opts.n_threads);;

    // Setup args & read input data: allocates memory for data
    prefix_sum_args_t *ps_args = alloc_args(opts.n_threads);

    // Setup pointer variable for input/output values
    int n_vals;
    int *input_vals, *output_vals;

    read_file(&opts, &n_vals, &input_vals, &output_vals);
    printf("Read n_vals: %d \n", n_vals);

    // Initialize chunk size as total number of values for sequential case
    int std_chunk_size = n_vals;
    int chunk_n_vals = n_vals;
    int prev_max = 0;

    if (opts.n_threads > 1) {
        std_chunk_size = (int) floor(n_vals/opts.n_threads);
        chunk_n_vals = (int) floor(n_vals/opts.n_threads);
    }

    //"op" is the operator you have to use, but you can use "add" to test
    int (*scan_operator)(int, int, int);
    scan_operator = op;
//    scan_operator = add;

    // Create an args worker packet for each thread
    fill_args(ps_args, &basic_barrier, opts.n_threads, n_vals, input_vals, output_vals,
        opts.spin, scan_operator, opts.n_loops, std_chunk_size, prev_max, chunk_n_vals);

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    if (sequential)  {
        printf("\nIn sequential mode...");
        printf("\nn_vals: %d", n_vals);

        //sequential prefix scan
        output_vals[0] = input_vals[0];
        for (int i = 1; i < n_vals; ++i) {
            //y_i = y_{i-1}  <op>  x_i
            printf("\n%d + %d", input_vals[i], output_vals[i-1]);
            output_vals[i] = scan_operator(output_vals[i-1], input_vals[i], ps_args->n_loops);
            printf(" = %d\n", output_vals[i]);
        }
    }
    else {
        printf("\nIn multi-thread mode...");

        // Create and launch treads with compute_prefix_sum() as thread worker function
        start_threads(threads, opts.n_threads, ps_args, compute_prefix_sum);

        printf("\nThreads started...");

        // Wait for threads to finish
        join_threads(threads, opts.n_threads);

        printf("\nThreads joined...");

        pthread_barrier_destroy(&basic_barrier);

    }

    //End timer and print out elapsed
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "time: " << diff.count() << std::endl;

    // Write output data
    write_file(&opts, &(ps_args[0]));

    printf("\nExited write_file...");

    // Free other buffers
    free(threads);
    free(ps_args);
}
