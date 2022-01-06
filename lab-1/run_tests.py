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
    parser.add_argument('--test', type=str, default='seq_64_test.txt', help='path to test data')
    parser.add_argument('--verbose', type=bool, default=False, help="turn on to print in-process log")
    args = parser.parse_args()
    return args

args = _parse_args()
print(args)


####### 16k multi-loops on Ubuntu via UTM virtual machine on M1 (8-core CPU) #######

# Setup thread & loop numbers as iterables
LOOPS = [1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150]
THREADS = [1, 2, 4, 6]
INPUTS = [args.test]

# THREADS = [args.threads]
# LOOPS = [args.loops]

# Initialize df for runtime across loop sizes
time_table = np.zeros((len(THREADS), len(LOOPS)))


for inp in INPUTS:
    for i_thread, thread in enumerate(THREADS):
        for i_loop, loop in enumerate(LOOPS):
            cmd = "./bin/prefix_scan -o temp.txt -n {} -i tests/{} -l {}".format(
                thread, inp, loop)
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

# Accuracy summary plot
for i_thread, thread in enumerate(THREADS):
    plt.plot(LOOPS, time_table[i_thread], label=thread)


plt.title("Runtime variation across Loops & Threads")
plt.ylabel("Runtime")
plt.xlabel('Loops')
plt.legend()
plt.show()


# csvs = []
# for inp in INPUTS:
#     for loop in LOOPS:
#         csv = ["{}/{}".format(inp, loop)]
#         for thr in THREADS:
#             # print("\nOn thread:", thr)
#             cmd = "./bin/prefix_scan -o temp.txt -n {} -i tests/{} -l {}".format(
#                 thr, inp, loop)
            
#             print("\ncmd:", cmd)

#             out = check_output(cmd, shell=True).decode("ascii")

#             if args.verbose:
#                 print("\nout:", out)

#             m = re.search("time: (.*)", out)
#             if m is not None:
#                 time = m.group(1)
#                 csv.append(time)
#                 print("\nRuntime for", loop,"-loop is", time)

#         csvs.append(csv)
#         sleep(0.5)

# header = ["microseconds"] + [str(x) for x in THREADS]

# print("\n")
# print(", ".join(header))
# for csv in csvs:
#     print (", ".join(csv))


