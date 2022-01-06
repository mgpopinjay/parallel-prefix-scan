#!/usr/bin/env python3
import os
from subprocess import check_output
import re
from time import sleep
import argparse


def _parse_args():
    """
    Command-line arguments to the system.
    :return: the parsed args bundle
    """
    parser = argparse.ArgumentParser(description='run_tests.py')

    # General system running and configuration options
    parser.add_argument('--threads', type=int, default=0, help='specify number of thread workers')
    parser.add_argument('--loops', type=int, default=10, help='specify number of loops')
    parser.add_argument('--test', type=str, default='seq_64_test.txt', help='path to test data')
    parser.add_argument('--verbose', type=bool, default=False, help="turn on to print in-process log")
    args = parser.parse_args()
    return args

args = _parse_args()
print(args)


####### 16k 10 loops on Ubuntu via UTM virtual machine on M1 (8-core CPU) #######
### 0 thread: 3784 4407
### 1 thread: 557  806
### 2 thread: 1041 2003
### 3 thread: 488  995
### 4 thread: 747  1042
### 5 thread: 1148 1261
### 6 thread:      
### 7 thread:
### 8 thread: 

####### 16k 100000 loops on Ubuntu via UTM virtual machine on M1 (8-core CPU) #######
### 0 thread: 2575890
### 1 thread: 2833714
### 2 thread: 1425316
### 3 thread: 978114
### 4 thread: 794636
### 5 thread: 675349
### 6 thread: 598844
### 7 thread: 544203
### 8 thread: 506094
### 9 thread: 588036
### 10 thread: 547236



THREADS = [args.threads]
LOOPS = [args.loops]
INPUTS = [args.test]

csvs = []
for inp in INPUTS:
    for loop in LOOPS:
        csv = ["{}/{}".format(inp, loop)]
        for thr in THREADS:
            # print("\nOn thread:", thr)
            cmd = "./bin/prefix_scan -o temp.txt -n {} -i tests/{} -l {}".format(
                thr, inp, loop)
            
            print("\ncmd:", cmd)

            out = check_output(cmd, shell=True).decode("ascii")

            if args.verbose:
                print("\nout:", out)

            m = re.search("time: (.*)", out)
            if m is not None:
                time = m.group(1)
                csv.append(time)
                print("\nRuntime for", loop,"-loop is", time)

        csvs.append(csv)
        sleep(0.5)

header = ["microseconds"] + [str(x) for x in THREADS]

print("\n")
print(", ".join(header))
for csv in csvs:
    print (", ".join(csv))


