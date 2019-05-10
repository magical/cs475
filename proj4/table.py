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
        row = line.split()
        data.append(row)

lengths = numpy.vectorize(lambda x: len(str(x)))(data)
colwidth = numpy.max(lengths, axis=0)

def printline():
    s = " " * colwidth[1] + " +-"
    for i in 3, 4:
        if i > 3:
            s += "-+-"
        if i >= 3:
            s += "-"*colwidth[i]
    print(s+"-+")

printline()
for row in data:
    label = row[1]
    row = row[3:5]
    s = ""
    for i, val in enumerate(row):
        s += " | " + str(val).rjust(colwidth[i])
    print(label.rjust(colwidth[1]) + s + " |")
printline()
