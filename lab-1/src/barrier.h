#ifndef _BARRIER_H
#define _BARRIER_H

#include <pthread.h>
#include <semaphore.h>


class Barrier {
    
    public:
        Barrier(int);
        virtual ~Barrier() { };
        virtual void wait() = 0;


    protected:
        int n_threads;
};


// Counter-based semaphroe barrier
class SemaphoreBarrier : public Barrier {
    
    public:
        SemaphoreBarrier(int);
        void wait();

        ~SemaphoreBarrier();

    private:
        int counter;    
        sem_t arrival;
        sem_t departure;
};

// Standard mutex-based pthread barrier
class PthreadBarrier : public Barrier {
    
    public:
        PthreadBarrier(int);
        void wait();

        ~PthreadBarrier();

    private:
        pthread_barrier_t barrier;
};


#endif
