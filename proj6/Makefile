CUDA_PATH	=	/usr/local/apps/cuda/cuda-10.0
CUDA_BIN_PATH	=	$(CUDA_PATH)/bin
CUDA_NVCC	=	$(CUDA_BIN_PATH)/nvcc

all: monteCarlo

%:	%.cu Makefile
	$(CUDA_NVCC) -o $@ $<

graphs:
	python2 graph.py proj6.data

test:
	for bs in 16 32 64; do \
		for size in 16 32 64 128 256 512; do \
			./monteCarlo "$$((size * 1024))" "$$bs" ;\
		done ;\
	done

files=monteCarlo.cu graph.py proj6.data *.h Makefile
zip:
	zip proj6.zip $(files)
