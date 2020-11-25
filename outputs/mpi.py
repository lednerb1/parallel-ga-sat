import numpy as np
import matplotlib.pyplot as plt
from math import sqrt, ceil
import sys
import os

procs, times = [1,2,4,8,16,24], []

with open('newHoly.dat', 'r') as file:
    counter = 0
    time = []
    for line in file:
        time.append(float(line))
        counter += 1
        if counter % 10 == 0:
            times.append(time)
            counter = 0
            time = []

print(*times, sep='\n')


meanTimes = [round(sum(time)/len(time),2) for time in times]
    
speedup = [meanTimes[0]/t for t in meanTimes]

plt.plot(procs, speedup, 'o-')
plt.title(f"3SAT speedup attempts with MPI on heterogeneous machines.\nGeneric Genetic Algorithm: POP=1k. GEN=100")
plt.ylabel('Speedup')
plt.xlabel('Processors')
# plt.xticks([0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170])
# plt.yticks([0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170])

# plt.gca().set_aspect('equal', adjustable='box')
plt.savefig('mpi_other.png')