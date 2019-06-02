with open("signal.txt") as f:
    f.readline()
    data = map(float, f)

sums = [0]*len(data)
for shift in range(len(data)):
    sum = 0
    for i in range(len(data)):
        sum += data[i] * data[(i+shift)%len(data)]
    sums[shift] = sum
    print shift, sum

