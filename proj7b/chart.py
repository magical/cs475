
import numpy

data = [ 32.51, 235.50, 124.83, 1531.17 ]
things = ["OpenMP-1", "OpenMP-8", "SIMD", "CUDA"]
colors = ["#4e79a7", "#f28e2b", "#e15759", "#76b7b2", "#59a14f", "#edc948", "#edc948", "#b07aa1", "#ff9da7", "#9c755f", "#bab0ac"] # tableau colors T10
x = numpy.arange(len(things))

import matplotlib; matplotlib.use('cairo')
import matplotlib.pyplot as plt
fig, ax = plt.subplots()
ax.bar(x, data, align='center', color=colors[:4])
ax.set_xticks(x)
ax.set_xticklabels(things)
ax.set_ylabel('KiloAutoCorrelates/second')
fig.savefig("perf.png")

