N=3
CXX=/usr/local/common/gcc-8.2.0/bin/g++
grain: grain.cpp Makefile
	$(CXX) -o $@ $< -O2 -lm -fopenmp -Wall -Wextra -ggdb

test: grain
	./grain

data: grain
	./grain | tee proj$N.data

table: table.txt
table.txt:
	python2 table.py proj$N.data > table.txt

graph: threads.png
threads.png: graph.py proj$N.data Makefile
	python2 graph.py proj$N.data

files=grain.cpp graph.py Makefile
zip: $(files)
	zip proj$N.zip $(files)
