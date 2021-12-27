#ifndef _ARGPARSE_H
#define _ARGPARSE_H

#include <getopt.h>
#include <stdlib.h>
#include <iostream>

struct options_t {
    char *in_file;
    char *out_file;
    int n_threads;
    int n_loops;
    bool spin;
};

void get_opts(int argc, char **argv, struct options_t *opts);
#endif
