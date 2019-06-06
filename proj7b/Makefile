CXX = /usr/local/common/gcc-6.3.0/bin/g++
CUDA_PATH	=	/usr/local/apps/cuda/cuda-10.0
CUDA_BIN_PATH	=	$(CUDA_PATH)/bin
CUDA_NVCC	=	$(CUDA_BIN_PATH)/nvcc

all: omp simd cuda

%:	%.cu read.cpp Makefile
	$(CUDA_NVCC) -o $@ $< read.cpp

%:	%.cpp read.o Makefile
	$(CXX) $(CXXFLAGS) -o $@ $< read.o -fopenmp -Wall -O2 -static-libstdc++

read.o: read.cpp Makefile

graphs:
	python2 plot.py p7cuda.data
	python2 chart.py

test:
	./omp 1 >p7omp1.data
	./omp 8 >p7omp8.data
	./simd >p7simd.data
	./cuda >p7cuda.data

files=omp.cpp simd.cpp cuda.cu read.cpp *.h plot.py chart.py *.data Makefile
zip:
	zip proj7b.zip $(files)
