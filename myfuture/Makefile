CXXFLAGS = -g -pthread -std=c++11
CXX = g++

all: main

main: main.cpp
	g++ -Wall -o2 -std=c++11 -pthread -g main.cpp -o main

run: main
	./main

valgrind: main
	valgrind --leak-check=full ./main

clean:
	rm -f main *.o *~