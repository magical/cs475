CUDA_PATH	=	/usr/local/apps/cuda/cuda-10.0
CUDA_BIN_PATH	=	$(CUDA_PATH)/bin
CUDA_NVCC	=	$(CUDA_BIN_PATH)/nvcc

all: arrayMul monteCarlo

%:	%.cu Makefile
	$(CUDA_NVCC) -o $@ $<

test:
	for bs in 16 32 64; do \
		for size in 16 32 64 128 256 512; do \
			./monteCarlo "$$((size * 1024))" "$$bs" ;\
		done ;\
	done
