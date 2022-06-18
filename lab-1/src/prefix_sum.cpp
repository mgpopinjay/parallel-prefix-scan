#include "prefix_sum.h"
#include "helpers.h"
#include <pthread.h>



void* compute_prefix_sum(void *a)
{
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
    // pthread_barrier_t  *basic_barrier;

    // For worker packet, use prefix_sum_args_t struct declared in helpers.h
    prefix_sum_args_t *args = (prefix_sum_args_t *) a;

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
    // basic_barrier = args->basic_barrier;

    printf("\n spin barrier: %d", spin);

    // PHASE 1
    // Get partial sum across chunks using sequential prefix sum
    printf("\n\n\nPhase 1 .....................");

    output_vals[0] = input_vals[0];
    for (int i = 1; i < chunk_n_vals; ++i) {
        //y_i = y_{i-1}  <op>  x_i
        output_vals[i] = op(output_vals[i-1], input_vals[i], n_loops);
    }


   // Exit if only a single thread
    if (n_threads == 1) {
        return 0;
    }

    // Check thread outputs before joining
    printf("\n\nThread %d sum at [%d] = %d", t_id, chunk_n_vals-1, output_vals[chunk_n_vals-1]);

    // FIRST BARRIER
    args->basic_barrier->wait();
    // pthread_barrier_wait(basic_barrier);


    printf("\n\nPhase 1 threads joined");

    // Check output vals before Phase 2
    printf("\n\nMid-way check: output[n_vals-1] = %d", output_vals[chunk_n_vals-1]);

 
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

            printf("\n");
            printf("\nThread %d:", i);
            printf("\nprior[%d] %d + current[%d] %d", prior_chunk_tail_ele, output_vals[prior_chunk_tail_ele], chunk_tail_ele, output_vals[chunk_tail_ele]);

            output_vals[chunk_tail_ele] = output_vals[prior_chunk_tail_ele] + output_vals[chunk_tail_ele];

            printf("\n = %d", output_vals[chunk_tail_ele]);

        }

        // If <= 2 threads
        final_tail_ele = n_vals - 1;
        chunk_tail_ele = final_tail_ele;

        prior_chunk_tail_ele = (n_threads - 1) * std_chunk_size - 1;

        printf("\n\nFinal element: %d + %d", output_vals[prior_chunk_tail_ele], output_vals[chunk_tail_ele]);

        output_vals[final_tail_ele] = output_vals[prior_chunk_tail_ele] + output_vals[chunk_tail_ele];

        printf("\n = %d", output_vals[final_tail_ele]);

//        printf("\n");
//        printf("\nPhase 2: output_vals %d = %d", final_tail_ele, output_vals[final_tail_ele]);

    }
    printf("\nEnd of Phase 2 BEFORE joining threads ");

    // SECOND BARRIER
    args->basic_barrier->wait();
    // pthread_barrier_wait(basic_barrier);

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
