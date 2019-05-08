all: first
%: %.cpp Makefile
	g++  -o $@ $< -O2 /usr/lib64/libOpenCL.so.1 -lm -fopenmp -Wall -Wno-deprecated-declarations
