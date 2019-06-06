import sys
filename = sys.argv[1]
data = []
with open(filename) as f:
    for line in f:
        n, d = line.split()
        data.append([int(n), float(d)])

import numpy
data = numpy.array(data)

import matplotlib; matplotlib.use('cairo')
import matplotlib.pyplot as plt
fig, ax = plt.subplots()
ax.scatter(data[:,0].flatten(), data[:,1].flatten())
ax.set(xlabel="shift")
fig.savefig("wave.png")

