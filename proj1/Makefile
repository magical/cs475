CC=/usr/local/common/gcc-8.2.0/bin/gcc
main: main.c
	$(CC) -o main main.c -lm -fopenmp -Wall -Wextra

test1: main
	./main 4

trials= 100 200 400 800 1600 3200 6400 20000 40000 100000
test: main
	for n in $(trials); do \
		for t in 1 2 4 8; do \
			./main $$t $$n ; \
		done ; \
	done | tee proj1.data

graph: threads.png
threads.png: graph.py proj1.data
	python2 graph.py proj1.data



allfiles=main.c graph.py proj1.data Makefile
zip: $(allfiles)
	zip proj1.zip $(allfiles)
