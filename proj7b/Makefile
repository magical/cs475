CUDA_PATH	=	/usr/local/apps/cuda/cuda-10.0
CUDA_BIN_PATH	=	$(CUDA_PATH)/bin
CUDA_NVCC	=	$(CUDA_BIN_PATH)/nvcc

all: cuda omp

%:	%.cu read.o Makefile
	$(CUDA_NVCC) -o $@ $< read.o

%:	%.cpp read.o Makefile
	$(CXX) $(CXXFLAGS) -o $@ $< read.o -fopenmp -Wall -O2

read.o: read.cpp Makefile

graphs:
	python2 graph.py proj6.data

test:
	./omp 1 >p7omp1.data
	./omp 8 >p7omp8.data
	#./simd > p7simd.data
	./cuda >p7cuda.data

files=cuda.cu plot.py *.h Makefile
zip:
	zip proj6.zip $(files)
