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
        year = row.pop(0)
        month = row.pop(0)
        date = "{}/{}".format(year, month.zfill(2))
        data.append([date] + row)

lengths = numpy.vectorize(lambda x: len(str(x)))(data)
colwidth = numpy.max(lengths, axis=0)

def printline():
    s = " " * colwidth[0] + " +-"
    for i, val in enumerate(data[0]):
        if i > 1:
            s += "-+-"
        if i >= 1:
            s += "-"*colwidth[i]
    print(s+"-+")

printline()
for row in data:
    label = row[0]
    row = row[1:]
    s = ""
    for i, val in enumerate(row):
        s += " | " + str(val).rjust(colwidth[i+1])
    print(label + s + " |")
printline()
