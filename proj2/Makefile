N=2
CC=/usr/local/common/gcc-8.2.0/bin/gcc
main: main.c
	$(CC) -o main main.c -lm -fopenmp -Wall -Wextra -O2

test: main
	./main 4 10

testall: main
	for n in `seq 100 100 10000` `seq 10000 1000 100000`; do \
		for t in 1 2 4 8; do \
			./main $$t $$n ; \
		done ; \
	done | tee proj$N.data

graph: threads.png
threads.png: proj$N.data
	python2 graph.py proj$N.data

zip: main.c Makefile
	zip proj$N.zip main.c graph.py Makefile
