#ifndef _BARRIER_H
#define _BARRIER_H

#include <pthread.h>
#include <semaphore.h>

class Barrier {
public:
    Barrier(int);
    virtual void wait() = 0;
    virtual ~Barrier() { };
protected:
    int n_threads;
};

class PthreadBarrier : public Barrier {
public:
    PthreadBarrier(int);
    void wait();
    ~PthreadBarrier();
private:
    pthread_barrier_t barrier;
};

class SemaphoreBarrier : public Barrier {
public:
    SemaphoreBarrier(int);
    void wait();
    ~SemaphoreBarrier();
private:
    sem_t arrival;
    sem_t departure;
    int counter;
};

#endif
