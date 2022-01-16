#include <barrier.h>

Barrier::Barrier(int n_threads) {
    this->n_threads = n_threads;
}

PthreadBarrier::PthreadBarrier(int n_threads) : Barrier(n_threads) {
    pthread_barrier_init(&barrier, NULL, n_threads);
}

void PthreadBarrier::wait() {
    pthread_barrier_wait(&barrier);
}

PthreadBarrier::~PthreadBarrier() {
    pthread_barrier_destroy(&barrier);
}

SemaphoreBarrier::SemaphoreBarrier(int n_threads) : Barrier(n_threads) {
    sem_init(&arrival, 0, 1);
    sem_init(&departure, 0, 0);
    counter = 0;
}

void SemaphoreBarrier::wait() {
    sem_wait(&arrival);
    if (++counter < n_threads) {
        sem_post(&arrival);
    }
    else {
        sem_post(&departure);
    }
    
    sem_wait(&departure);
    if (--counter > 0) {
        sem_post(&departure);
    }
    else {
        sem_post(&arrival);
    }
}

SemaphoreBarrier::~SemaphoreBarrier() {
    sem_destroy(&arrival);
    sem_destroy(&departure);
}
