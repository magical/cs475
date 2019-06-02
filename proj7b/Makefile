CUDA_PATH	=	/usr/local/apps/cuda/cuda-10.0
CUDA_BIN_PATH	=	$(CUDA_PATH)/bin
CUDA_NVCC	=	$(CUDA_BIN_PATH)/nvcc

all: cuda

%:	%.cu Makefile
	$(CUDA_NVCC) -o $@ $<

graphs:
	python2 graph.py proj6.data

test:
	#./omp 8 > p7omp.data
	#./simd > p7omp.data
	./cuda > p7cuda.data

files=cuda.cu plot.py *.h Makefile
zip:
	zip proj6.zip $(files)
