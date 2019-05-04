#!/usr/bin/env python2
# encoding: utf-8

import sys
import matplotlib
matplotlib.use('cairo')
import matplotlib.pyplot as plt
import numpy


colors = ["#4e79a7", "#f28e2b", "#e15759", "#76b7b2", "#59a14f", "#edc948", "#edc948", "#b07aa1", "#ff9da7", "#9c755f", "#bab0ac"] # tableau colors T10
markers = list("osD^vp*|_+")


filename = sys.argv[1]
data = []
with open(filename) as f:
    for line in f:
        row = [float(x) for x in line.split()]
        data.append(row)

data = numpy.array(data)

def do_plot(x, ys, xlabel='month', ylabels=[], outfilename='figure.png', logscale=False):
    fig, ax = plt.subplots()

    x_formatter = matplotlib.ticker.ScalarFormatter(useOffset=False)
    ax.xaxis.set_major_formatter(x_formatter)

    if logscale:
        ax.set_xscale('log')
        ax.set_xticks(x)
        ax.set_xticklabels(x)
    lines = []
    for y, c, m in zip(ys, colors, markers):
        l, = ax.plot(x, y, c = c, marker=m, linewidth=2.5)
        lines.append(l)
    ax.set(xlabel = xlabel, ylabel = '')
    if ylabels:
        ax.legend(lines, ylabels,
                    loc='best',
                    fontsize = 'small',
                    #ncol=4, loc='upper center',
                    #bbox_to_anchor=[0.5, 1.1],
                    fancybox=True, shadow=True)
    fig.savefig(outfilename)

def plural(n, singular, plural):
    return singular if n == 1 else plural

def addunits(xs, unit, plural_unit):
    return ["%s %s" % (x, plural(x, unit, plural_unit)) for x in xs]

labels = [
    u"temperature (°C)",
    u"precipitation (in)",
    u"number of graindeer",
    u"grain height (in)",
    u"grain density factor",
]

x = data[: , 0]  + (data[: , 1] / 12.0)
ys = numpy.transpose(data[: , 2:])
do_plot(x, ys, xlabel='year', ylabels=labels, outfilename="figure.png")
