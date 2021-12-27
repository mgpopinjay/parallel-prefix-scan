#!/usr/bin/env python3
import os
from subprocess import check_output
import re
from time import sleep

#
#  Feel free (a.k.a. you have to) to modify this to instrument your code
#

### 0 thread: 2016
### 1 thread: 63     // fix by early exit after phase 1
### 2 thread: 2016   // fix by indexing by chunk_n_vals in each thread
### 3 thread: 6048
### 4 thread: 12096
### 5 thread: 24192
### 6 thread: 48384
### 7 thread: 96768
### 8 thread: 193536

THREADS = [4]
# LOOPS = [1, 10]
LOOPS = [1]
INPUTS = ["seq_64_test.txt"]

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
            print("\nout type:", type(out))
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
