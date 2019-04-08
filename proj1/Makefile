CC=/usr/local/common/gcc-8.2.0/bin/gcc
main: main.c
	$(CC) -o main main.c -lm -fopenmp

zip: main.c Makefile
	zip proj1.zip main.c Makefile
