N=3
CXX=/usr/local/common/gcc-8.2.0/bin/g++
grain: grain.cpp Makefile
	$(CXX) -o $@ $< -O2 -lm -fopenmp -Wall -Wextra -ggdb -fsanitize=thread -Wl,-rpath=/usr/local/common/gcc-8.2.0/lib64

test: grain
	./grain

data: main
	./grain | tee proj$N.data

graph: threads.png
threads.png: graph.py proj$N.data
	python2 graph.py proj$N.data

zip: main.c Makefile
	zip proj$N.zip main.c graph.py Makefile
