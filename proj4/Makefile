N=4
CXX=/usr/local/common/gcc-8.2.0/bin/g++

simd: main.c simd.p4.cpp simd.p4.h Makefile
	$(CXX) -o $@ main.c simd.p4.cpp -O2 -lm -fopenmp -Wall -Wextra -ggdb

test: simd
	./simd

data: simd
	for n in $$(seq 8 8 120) $$(seq 120 40 1000) $$(seq 1000 400 10000) $$(seq 10000 1000 100000); do \
		./simd 1 $$n; \
	done | tee proj$N.data

graph: threads.png
threads.png: graph.py proj$N.data Makefile
	python2 graph.py proj$N.data

files=main.c simd.p4.* Makefile
zip: $(files)
	zip proj$N.zip $(files)
