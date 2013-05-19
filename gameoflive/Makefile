all:	build

build: main.cpp
	g++ -Wall -o2 -std=c++11 -pthread -g main.cpp -o main -lrt

run:
	mkdir ./frames
	./main

clean:
	rm -rf ./frames
	rm ./main
