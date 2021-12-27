#include "prefix_sum.h"
#include "helpers.h"
#include <pthread.h>


// ------------------------------------------------------------------------------
// START OF REFERENCE BARRIER
// ------------------------------------------------------------------------------


// These three global variables are used in the implementation of a simple counter barrier
pthread_cond_t end_of_phase;   // condition variable - waiting for all threads to reach barrier
pthread_mutex_t phase_barrier; // mutex lock - to ensure atomicity of barrier function
int number_complete_phase;     // count variable - the number of threads currently at the barrier


void barrier(int n_threads){
    // Acquire the lock and increment the global number_complete variable
    pthread_mutex_lock(&phase_barrier);
    number_complete_phase++;
    // If I am the last thread then reset the global variable and release the other waiting threads
    if (number_complete_phase == n_threads) {
        number_complete_phase = 0;
        pthread_cond_broadcast(&end_of_phase);
    }
        // Otherwise wait until the final thread gets here
        // Automatically release the lock
    else {
        pthread_cond_wait(&end_of_phase, &phase_barrier);
    }
    // Explicitly release the lock
    pthread_mutex_unlock(&phase_barrier);
}
// ------------------------------------------------------------------------------
// END OF REFERENCE BARRIER
// ------------------------------------------------------------------------------


void* compute_prefix_sum(void *a)
{
    /************************
     * Your code here...    *
     * or wherever you like *
     ************************/
    int                *input_vals;
    int                *output_vals;
    bool               spin;
    int                n_vals;
    int                n_threads;
    int                t_id;
    int                (*op)(int, int, int);
    int                n_loops;
    int                std_chunk_size;
    int                *prev_max;
    int                chunk_n_vals;

    // For worker packet, use prefix_sum_args_t struct declared in helpers.h
    prefix_sum_args_t *args = (prefix_sum_args_t *) a;

//    input_vals = args->*input_vals;
//    output_vals = args->*output_vals;
    input_vals = args->input_vals;
    output_vals = args->output_vals;
    spin = args->spin;
    n_vals = args->n_vals;
    n_threads = args->n_threads;
    t_id = args->t_id;
    op = args->op;
    n_loops = args->n_loops;
    std_chunk_size = args->std_chunk_size;
    prev_max = args->prev_max;             // Pointer asterisk for args necessary?
    chunk_n_vals = args->chunk_n_vals;



    // PHASE 1
    // Get partial sum across chunks using sequential prefix sum
    printf("\n\n\nPhase 1 .....................");

    output_vals[0] = input_vals[0];
    for (int i = 1; i < chunk_n_vals; ++i) {
        //y_i = y_{i-1}  <op>  x_i
        output_vals[i] = op(output_vals[i-1], input_vals[i], n_loops);

//        printf("\nPhase 1: n_vals %d = %d", i, output_vals[i]);
    }

    // Check thread outputs before joining
    printf("\n");
    printf("\nThread %d sum at [%d] = %d", t_id, chunk_n_vals-1, output_vals[chunk_n_vals-1]);


    barrier(n_threads);

    printf("\n");
    printf("\nPhase 1 threads joined");

    // Check output vals before Phase 2
    printf("\n");
    printf("\nMid-way check: output[n_vals-1] = %d", output_vals[chunk_n_vals-1]);

    // Exit if only a single thread
    if (n_threads == 1) {
        return 0;
    }

    // PHASE 2
    // Using just thread worker 0, compute pre-fix sum with partial sum from each chunk
    printf("\n\n\nPhase 2 .....................");

    if (t_id == 0) {
        printf("\nt_id = %d", t_id);

        int chunk_tail_ele;
        int prior_chunk_tail_ele;
        int final_tail_ele;

        // If > 2 threads
        for (int i = 1; i < n_threads-1; i++) {
            chunk_tail_ele = ( (i + 1) * std_chunk_size) - 1;
            prior_chunk_tail_ele = (i * std_chunk_size) - 1;
            output_vals[chunk_tail_ele] = output_vals[prior_chunk_tail_ele] + output_vals[chunk_tail_ele];

            printf("\n");
            printf("\nprior_chunk_tail_ele: %d", prior_chunk_tail_ele);
            printf("\nPhase 2: chunk_tail %d = %d", chunk_tail_ele, output_vals[chunk_tail_ele]);

        }

        // If <= 2 threads
        final_tail_ele = n_vals - 1;
        chunk_tail_ele = final_tail_ele;

        prior_chunk_tail_ele = (n_threads - 1) * std_chunk_size - 1;
        output_vals[final_tail_ele] = output_vals[prior_chunk_tail_ele] + output_vals[chunk_tail_ele];

        printf("\n");
        printf("\nPhase 2: output_vals %d = %d", final_tail_ele, output_vals[final_tail_ele]);

    }
    printf("\nEnd of Phase 2 BEFORE joining threads ");


    barrier(n_threads);

    printf("\n");
    printf("\nEnd of Phase 2 AFTER joining threads");

    // PHASE 3
    // Use remaining thread workers to update all elements except the final element
    printf("\n\n\nPhase 3 .....................");

    if (t_id != 0) {
        printf("\nt_id = %d", t_id);
        printf("\nprev_max = %d", *prev_max);

        for (int i = 0; i < chunk_n_vals-1; i++) {
            output_vals[i] = output_vals[i] + *prev_max;
        }
    }

    printf("\nEnd of Phase 3");


    return 0;
}
