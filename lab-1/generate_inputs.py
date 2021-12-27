#!/usr/bin/env python3
import random
import os

K = 1024

INPUTS = {
    "1k.txt": 1*K,
    "8k.txt": 8*K,
    "16k.txt": 16*K,
}

try:
    os.mkdir("tests")
except:
    pass

for name, sz in INPUTS.items():
    with open(os.path.join("tests", name), 'w') as f:
        f.write("{}\n".format(sz))
        n = sz
        while n > 0:
            gen = min(n, 64*K)
            ar = [str(random.randint(0, 100000)) for _ in range(gen)]
            f.write("\n".join(ar))
            n -= gen

    with open(os.path.join("tests", "seq_64_test.txt"), 'w') as f:
        f.write("64\n")
        for i in range(64):
            f.write("{}\n".format(i))

    