#include "operators.h"

int __attribute__ ((noinline)) op(int a, int b, int n_loop) {
    volatile int acc = 0;
    for (int i = 0; i < n_loop; i++) {
        acc++;
    }
    return (a+b)*(acc/n_loop);
}


int add(int a, int b, int __) {
    return a+b;
}