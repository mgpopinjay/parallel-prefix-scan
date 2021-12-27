#include "threads.h"
#include "helpers.h"

pthread_t *alloc_threads(int n_threads)
{
  return (pthread_t *)malloc(n_threads * sizeof(pthread_t));
}

void start_threads(pthread_t *threads,
                   int n_threads,
                   struct prefix_sum_args_t *args,
                   void *(*start_routine)(void *)) {
  int ret = 0;
  for (int i = 0; i < n_threads; ++i) {
    ret |= pthread_create(&(threads[i]), NULL, start_routine,
                          (void *)&(args[i]));
  }

  if (ret) {
    std::cerr << "Error starting threads" << std::endl;
    exit(1);
  }
}

void join_threads(pthread_t *threads,
                  int n_threads) {
  int res = 0;
  for (int i = 0; i < n_threads; ++i) {
    int *ret;
    res |= pthread_join(threads[i], (void **)&ret);
  }

  if (res) {
    std::cerr << "Error joining threads" << std::endl;
    exit(1);
  }
}
