import numpy as np
import matplotlib.pyplot as plt
from math import sqrt
import sys
import os

runs = dict()

for root, dir, files in os.walk("schedulers"):
    if len(dir) != 0: continue
    files = sorted(files)
    for file in files:
        if '.png' in file: continue
        with open(root + "/" + file, 'r') as f:
            values = []
            mean = 0
            for line in f:
                values.append(int(line))
                mean += int(line)
            mean = float(mean) / len(values)
            stdev = 0
            for v in values:
                stdev += (v-mean)**2
            stdev = sqrt(float(stdev) / len(values))
            key = root + "/" + file.split('.')[0]
            try:
                runs[key]['mean'].append(mean)
            except KeyError:
                runs[key] = dict()
                runs[key]['mean'] = [mean]
            try:
                runs[key]['std'].append(stdev)
            except KeyError:
                runs[key]['std'] = [stdev]

lastKey = None
handles = []
for key in runs:
    if key.split('/')[1] != lastKey:
        if lastKey is not None:
            plt.legend(handles=handles)
            plt.savefig(f"schedulers/{lastKey}/out.png")
        plt.clf()
        handles = []
        lastKey = key.split('/')[1]
        
    it = runs[key]['mean']
    h, = plt.plot(range(1,6), [it[0]/i for i in it], 'o-', label="".join(key.split('/')[1:]))
    handles.append(h)
    # plt.errorbar(range(1,6), runs[key]['std'], linestyle='None', marker='^')
    plt.title(f"3SAT speedup attempts on i5 @ 3.5GHz w/ 6 cores. schedule({key.split('/')[1]})\nGeneric Genetic Algorithm: POP=100. GEN=10k")
    plt.ylabel('Speedup')
    plt.xlabel('Threads')
    plt.yticks([1,2,3,4,5])
    
plt.legend(handles=handles)
plt.savefig(f"schedulers/{lastKey}/out.png")