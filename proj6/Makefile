CUDA_PATH	=	/usr/local/cuda-10.1
CUDA_BIN_PATH	=	$(CUDA_PATH)/bin
CUDA_NVCC	=	$(CUDA_BIN_PATH)/nvcc

all: arrayMul monteCarlo

%:	%.cu Makefile
	$(CUDA_NVCC) -o $@ $<
