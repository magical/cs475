N=3
CXX=/usr/local/common/gcc-8.2.0/bin/g++
grain: grain.cpp
	$(CXX) -o $@ $< -lm -fopenmp -Wall -Wextra -O2 -fsanitize=thread

test: grain
	./grain

data: main
	./grain | tee proj$N.data

graph: threads.png
threads.png: graph.py proj$N.data
	python2 graph.py proj$N.data

zip: main.c Makefile
	zip proj$N.zip main.c graph.py Makefile
