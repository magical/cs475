with open("proj2.data") as f:
    lines = list(f)

lines.reverse()
lines = lines[:4]
#lines = lines[4:8]
p1 = 0.0
p2 = 0.0
p4 = 0.0
p8 = 0.0
for l in lines:
    row = l.split()
    if row[0] == '1':
        p1 = float(row[3])
    elif row[0] == '2':
        p2 = float(row[3])
    elif row[0] == '4':
        p4 = float(row[3])
    elif row[0] == '8':
        p8 = float(row[3])

print(p1, p2, p4, p8)
for n, pn in zip([2,4,8], [p2, p4, p8]):
    S = pn/p1
    Fp = (1 - 1/S)/(1 - 1/float(n))
    Smax = 1/(1-Fp)
    print(n, "threads")
    print("speedup = %.2f" % S)
    print("parallel fraction = %.4f" % Fp)
    print("max speedup = %.2f" % Smax)
    print()
