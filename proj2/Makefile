N=2
CC=/usr/local/common/gcc-8.2.0/bin/gcc
main: main.c
	$(CC) -o main main.c -lm -fopenmp -Wall -Wextra -O2

test: main
	./main 4 10

testall: main
	for n in 8 16 32 64 128 256 512 1024; do \
		for t in 1 2 4 8; do \
			./main $$t $$n ; \
		done ; \
	done | tee proj$N.data

graph: threads.png
threads.png: graph.py proj$N.data
	python2 graph.py proj$N.data

files=main.c graph.py table.py fraction.py proj$N.data Makefile
zip: $(files)
	zip proj$N.zip $(files)
