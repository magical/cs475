#!/usr/bin/env python2

import sys
import matplotlib
matplotlib.use('cairo')
import matplotlib.pyplot as plt
import numpy

colors = ["#4e79a7", "#f28e2b", "#e15759", "#76b7b2", "#59a14f", "#edc948", "#edc948", "#b07aa1", "#ff9da7", "#9c755f", "#bab0ac"] # tableau colors T10
markers = list("osD^vp*|_+")

filename = sys.argv[1]
filename2 = sys.argv[2]
prefix = sys.argv[3]

def read_data(filename):
    data = []
    with open(filename) as f:
        # kinda wish python had scanf
        for line in f:
            n, l, wg, perf, _units = line.split()
            n = int(n)
            l = int(l)
            wg = int(wg)
            perf = float(perf)
            data.append((n, l, wg, perf))

    rawdata = data

    numglobal = sorted(set(x[0] for x in data))
    numlocal = sorted(set(x[1] for x in data))

    data = numpy.zeros((len(numglobal), len(numlocal)))
    for t, n, _, perf in rawdata:
        ti = numglobal.index(t)
        ni = numlocal.index(n)
        data[ti][ni] = perf

    return data, numglobal, numlocal

data, numglobal, numlocal = read_data(filename)
data2, _, _ = read_data(filename2)
assert data.shape == data2.shape

# FIXME truncate nodes other than the first 10
numlocal = numlocal[:10]
data = data[:,:10]
data2 = data2[:,:10]

def do_plot(x, ys, ys2, xlabel='threads', ylabels=[], outfilename='figure.png', logscale=False, override_xticks=False):
    fig, ax = plt.subplots()
    if override_xticks:
        ax.set_xticks(x)
    ax.set_ylim(0, 22)
    if logscale:
        ax.set_xscale('log')
        ax.set_xticks(x)
        ax.set_xticklabels(x)
    lines = []
    for y, c, m in zip(ys, colors, markers):
        l, = ax.plot(x, y, c = c, marker=m)
        #_ = ax.scatter(x, y, c = c, marker=m)
        lines.append(l)
    lines2 = []
    for y, c, m in zip(ys2, colors, markers):
        l, = ax.plot(x, y, c=c, marker=m, linestyle='--')
        lines2.append(l)
    ax.set(xlabel = xlabel, ylabel = 'performance (billion ops/s)')
    if ylabels:
        ax.legend(lines, ylabels, ncol=4, loc='best',
                    fancybox=True, shadow=True)
        #ax.legend([lines[0], lines2[0]], ["Multiply", "Multiply-add"], loc="best",
        #        fancybox=True, shadow=True)
    fig.savefig(outfilename)

print(numglobal)
print(numlocal)
print(data)
print(data2)

global_labels = ["%sK" % (x/1024) if x < (1<<20) else "%sM" % (x/1024/1024)  for x in numglobal]
do_plot(numpy.array(numglobal)/1024,
    numpy.transpose(data), numpy.transpose(data2),
    xlabel='Global work size (Ki)', ylabels=map(str, numlocal),
    outfilename=prefix+"-global.png")
do_plot(numlocal, data, data2,
    xlabel='Local work size (elements)', ylabels=global_labels,
    outfilename=prefix+"-local.png", override_xticks=True)

def print_table(data):
    lengths = numpy.vectorize(lambda x: len(str(x)))(data)
    colwidth = numpy.max(lengths, axis=0)
    labelwidth = max(len(str(x)) for x in numglobal)

    def printline():
        s = " " * labelwidth + " +-"
        for i in range(len(data[0])):
            if i > 0:
                s += "-+-"
            if i >= 0:
                s += "-"*colwidth[i]
        print(s+"-+")

    s = ""
    for i, val in enumerate(numlocal):
        s += "   " + str(val).rjust(colwidth[i])
    print(" " * labelwidth + s)
    printline()
    for j, row in enumerate(data):
        label = str(numglobal[j]).rjust(labelwidth)
        s = ""
        for i, val in enumerate(row):
            s += " | " + str(val).rjust(colwidth[i])
        print(label + s + " |")
    printline()

print_table(data)
print_table(data2)
