import matplotlib
matplotlib.use('cairo')
import matplotlib.pyplot as plt
import numpy

t = numpy.array([1, 2, 4, 8])
perf = numpy.array([18.019045,
36.096458,
72.066689,
143.180038,
])

fig, ax = plt.subplots()
ax.plot(t, perf)
ax.scatter(t, perf)
ax.set(xlabel = 'threads', ylabel = 'perfomance (M trials/s)')
fig.savefig("threads.png")
#plt.show()
