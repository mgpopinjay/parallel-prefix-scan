#!/usr/bin/env python3
import os
from subprocess import check_output
import re
from time import sleep
import argparse
import numpy as np
import matplotlib.pyplot as plt


def _parse_args():
    """
    Command-line arguments to the system.
    :return: the parsed args bundle
    """
    parser = argparse.ArgumentParser(description='run_tests.py')

    # General system running and configuration options
    # parser.add_argument('--threads', type=int, default=0, help='specify number of thread workers')
    # parser.add_argument('--loops', type=int, default=10, help='specify number of loops')
    parser.add_argument('--test', type=str, default='16k.txt', help='path to test data')
    parser.add_argument('--spin', type=bool, default=False, help="flag for custom barrier activation")
    parser.add_argument('--verbose', type=bool, default=False, help="turn on to print in-process log")
    args = parser.parse_args()
    return args

args = _parse_args()
print(args)



def main():

    # comp_loops()
    comp_threads()


def comp_loops():
    """
    Plot 10 vs 100k loops
    """

    # THREADS = [args.threads]
    # LOOPS = [args.loops]

    # Setup thread & loop numbers as iterables
    LOOPS = [10, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000]
    # LOOPS = [10, 100, 200, 300, 400, 500]
    # LOOPS = [10, 100000]
    THREADS = [1, 2, 4, 8, 16]
    INPUTS = [args.test]

    # Initialize df for runtime across loop sizes
    time_table = np.zeros((len(THREADS), len(LOOPS)))


    for inp in INPUTS:
        for i_thread, thread in enumerate(THREADS):
            for i_loop, loop in enumerate(LOOPS):
                print("\Computing file:", inp)
                cmd = "./bin/prefix_scan -o temp.txt -n {} -i tests/{} -l {} -s {}".format(
                    thread, inp, loop, args.spin)
                print("\ncmd:", cmd)
                out = check_output(cmd, shell=True).decode("ascii")
                if args.verbose:
                    print("\nout:", out)
                m = re.search("time: (.*)", out)
                if m is not None:
                    time = m.group(1)
                    time_table[i_thread, i_loop] = time
                    print("\nRuntime for", loop,"-loop on", thread, "-thread is", time)
            sleep(0.5)         

    print(time_table)


    # Plot
    for i_thread, thread in enumerate(THREADS):
        plt.plot(LOOPS, time_table[i_thread], label=thread)

    if args.spin == True:
        plt.title("Runtime across Loops & Threads (Semaphore)")
    else:
        plt.title("Runtime across Loops & Threads (Mutex)")

    plt.ylabel("Runtime")
    plt.xlabel('Loops')
    plt.legend()
    plt.show()



def comp_threads():
    """
    Plots 100k loops over 2, 4, 6 ... to 16 threads
    """
    LOOPS = [100000]
    THREADS = [1, 2, 4, 6, 8, 10, 12, 14, 16]
    INPUTS = [args.test]

    # Initialize df for runtime across thread numbers
    time_table = np.zeros((len(LOOPS), len(THREADS)))


    for inp in INPUTS:
        for i_loop, loop in enumerate(LOOPS):
            for i_thread, thread in enumerate(THREADS):
                print("\Computing file:", inp)
                cmd = "./bin/prefix_scan -o temp.txt -n {} -i tests/{} -l {} -s {}".format(
                    thread, inp, loop, args.spin)
                print("\ncmd:", cmd)
                out = check_output(cmd, shell=True).decode("ascii")
                if args.verbose:
                    print("\nout:", out)
                m = re.search("time: (.*)", out)
                if m is not None:
                    time = m.group(1)
                    time_table[i_loop, i_thread] = time
                    print("\nRuntime for", loop,"-loop on", thread, "-thread is", time)

            sleep(0.5)            

    print(time_table)

    # Plot
    for i_loop, loop in enumerate(LOOPS):
        plt.plot(THREADS, time_table[i_loop], label=loop)


    if args.spin == True:
        plt.title("Runtimes across Threads Numbers (Semaphore)")
    else:
        plt.title("Runtimes across Threads Numbers (Mutex)")

    plt.ylabel("Runtime")
    plt.xlabel('Threads')
    plt.legend()
    plt.show()


if __name__ == "__main__":
    main()
