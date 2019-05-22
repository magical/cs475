#CXX=/usr/local/common/gcc-8.2.0/bin/g++
#OPENCL=/usr/lib64/libOpenCL.so.1
#OPENCL=./libOpenCL.so.1
OPENCL=-lOpenCL

all: first muladd mulreduce
%: %.cpp Makefile
	$(CXX)  -o $@ $< -O2 $(OPENCL) -lm -fopenmp -Wall -Wno-deprecated-declarations -ggdb -Wl,-rpath=.

data: first
	for local in 32 64 128 256 512; do \
		for n in 1K 4K 16K 64K 512K 1M 8M 64M; do \
			./first $$n $$local ; \
		done ; \
	done | tee proj5.data
data2: muladd
	for local in 32 64 128 256 512; do \
		for n in 1K 4K 16K 64K 512K 1M 8M 64M; do \
			./muladd $$n $$local ; \
		done ; \
	done | tee p5muladd.data
data3: mulreduce
	for local in 32 64 128 256 512; do \
		for n in 1K 4K 16K 64K 512K 1M 8M 64M; do \
			./mulreduce $$n $$local ; \
		done ; \
	done | tee p5mulreduce.data

graphs:
	python graph2.py proj5.data p5muladd.data combined
	python graph.py p5mulreduce.data p5mulreduce

files=*.cpp *.cl graph.py graph2.py Makefile
zip: $(files)
	zip proj5.zip $(files)
