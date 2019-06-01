#!/usr/bin/env python2

import sys
import matplotlib
matplotlib.use('cairo')
import matplotlib.pyplot as plt
import numpy


filename = sys.argv[1]
data = []
with open(filename) as f:
    # kinda wish python had scanf
    for line in f:
        t, n, prob, perf = line.split()
        t = int(t)
        n = int(n)
        prob = float(prob)
        perf = float(perf)
        data.append((t, n, prob, perf))

rawdata = data

blocksizes = sorted(set(x[0] for x in data))
numtrials = sorted(set(x[1] for x in data))

data = numpy.zeros((len(blocksizes), len(numtrials)))
for t, n, _, perf in rawdata:
    ti = blocksizes.index(t)
    ni = numtrials.index(n)
    data[ti][ni] = perf

# FIXME truncate trials other than the first 10
numtrials = numtrials[:10]
data = data[:,:10]


# doesn't work; matplotlib is too old and doesn't support colors greater than C1
#colors = ["C%d" % n for n in range(10)]
colors = ["#4e79a7", "#f28e2b", "#e15759", "#76b7b2", "#59a14f", "#edc948", "#edc948", "#b07aa1", "#ff9da7", "#9c755f", "#bab0ac"] # tableau colors T10
markers = list("osD^vp*|_+")



def do_plot(x, ys, xlabel='threads', ylabels=[], outfilename='figure.png', logscale=False):
    fig, ax = plt.subplots()
    if logscale:
        ax.set_xscale('log')
        ax.set_xticks(x)
        ax.set_xticklabels(x)
    lines = []
    for y, c, m in zip(ys, colors, markers):
        l, = ax.plot(x, y, c = c, marker=m)
        #_ = ax.scatter(x, y, c = c, marker=m)
        lines.append(l)
    ax.set(xlabel = xlabel, ylabel = 'perfomance (M trials/s)')
    if ylabels:
        fig.legend(lines, ylabels, ncol=4, loc='upper center',
                    #bbox_to_anchor=[0.5, 1.1],
                    #columnspacing=1.0, labelspacing=0.0,
                    #handletextpad=0.0, handlelength=1.5,
                    fancybox=True, shadow=True)
    fig.savefig(outfilename)

def plural(n, singular, plural):
    return singular if n == 1 else plural

def addunits(xs, unit, plural_unit):
    return ["%s %s" % (x, plural(x, unit, plural_unit)) for x in xs]

#matplotlib.style.use('default')
print(blocksizes)
print(data)
do_plot(blocksizes, numpy.transpose(data), xlabel='blocksize', ylabels=addunits(numtrials, "trial", "trials"), outfilename="blocks.png")
do_plot(numtrials, data, xlabel='trials', ylabels=['blocksize=%d'%x for x in blocksizes], outfilename="trials.png")
#do_plot(numtrials, data, xlabel='trials', ylabels=addunits(blocksizes, "blocksize", "blocksize"), outfilename="trials-log.png", logscale=True)


def print_table(data):
    data = numpy.transpose(data)
    lengths = numpy.vectorize(lambda x: len(str(x)))(data)
    colwidth = numpy.max(lengths, axis=0)
    labelwidth = max(len(str(x)) for x in numtrials)

    def printline():
        s = " " * labelwidth + " +-"
        for i in range(len(data[0])):
            if i > 0:
                s += "-+-"
            if i >= 0:
                s += "-"*colwidth[i]
        print(s+"-+")

    s = ""
    for i, val in enumerate(blocksizes):
        s += "   " + str(val).rjust(colwidth[i])
    print(" " * labelwidth + s)
    printline()
    for j, row in enumerate(data):
        label = str(numtrials[j]).rjust(labelwidth)
        s = ""
        for i, val in enumerate(row):
            s += " | " + str(val).rjust(colwidth[i])
        print(label + s + " |")
    printline()

print_table(data)
