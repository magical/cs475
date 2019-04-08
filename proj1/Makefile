CC=/usr/local/common/gcc-8.2.0/bin/gcc
main: main.c
	$(CC) -o main main.c -lm -fopenmp -Wall -Wextra

test: main
	@./main 1
	@./main 2
	@./main 4
	@./main 8

zip: main.c Makefile
	zip proj1.zip main.c Makefile
