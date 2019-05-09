all: first
%: %.cpp Makefile
	g++  -o $@ $< -O2 -lOpenCL -lm -fopenmp -Wall -Wno-deprecated-declarations
