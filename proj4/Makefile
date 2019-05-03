N=3
CXX=/usr/local/common/gcc-8.2.0/bin/g++

simd: main.c simd.p4.cpp simd.p4.h Makefile
	$(CXX) -o $@ main.c simd.p4.cpp -O2 -lm -fopenmp -Wall -Wextra -ggdb

test: simd
	./simd

data: simd
	./simd | tee proj$N.data

graph: threads.png
threads.png: graph.py proj$N.data Makefile
	python2 graph.py proj$N.data

files=main.c simd.p4.* Makefile
zip: $(files)
	zip proj$N.zip $(files)
