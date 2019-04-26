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

numthreads = sorted(set(x[0] for x in data))
numnodes = sorted(set(x[1] for x in data))

data = numpy.zeros((len(numthreads), len(numnodes)))
for t, n, _, perf in rawdata:
    ti = numthreads.index(t)
    ni = numnodes.index(n)
    data[ti][ni] = perf

data = numpy.transpose(data)

lengths = numpy.vectorize(lambda x: len(str(x)))(data)
colwidth = numpy.max(lengths, axis=0)

def printline():
    s = ""
    for i, val in enumerate(data[0]):
        if i != 0:
            s += "-+-"
        s += "-"*colwidth[i]
    print("     +-"+s+"-+")

printline()
for n, row in zip(numnodes, data):
    s = ""
    for i, val in enumerate(row):
        if i != 0:
            s += " | "
        s += str(val).rjust(colwidth[i])
    print(str(n).rjust(4) + " | "+s+" |")
printline()
