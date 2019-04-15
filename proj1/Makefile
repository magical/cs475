CC=/usr/local/common/gcc-8.2.0/bin/gcc
main: main.c
	$(CC) -o main main.c -lm -fopenmp -Wall -Wextra

test1: main
	./main 4

test: main
	for n in `seq 100 100 10000` `seq 10000 1000 100000`; do \
		for t in 1 2 4 8; do \
			./main $$t $$n ; \
		done ; \
	done | tee proj1.data

graph: threads.png
threads.png: proj1.data
	python2 graph.py proj1.data



zip: main.c Makefile
	zip proj1.zip main.c Makefile
