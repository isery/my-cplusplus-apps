CXXFLAGS = -Wall -g -pthread -fopenmp
LDFLAGS = -g -pthread -lm -fopenmp 
CCFLAGS = -Wall -g

all: main pthread boost_thread omp

main: main.o
	$(CXX) $(LDFLAGS) -o $@ $< -lrt -lboost_thread

pthread: pthread.o
	$(CXX) $(LDFLAGS) -o $@ $< -lrt -lboost_thread

boost_thread: boost_thread.o
	$(CXX) $(LDFLAGS) -o $@ $< -lrt -lboost_thread

omp: omp.o
	$(CC) -o $@ omp.o -lm -lgomp -lrt -lboost_thread

clean:
	rm -f main pthread boost_thread omp *.o *~

ppm:
	rm -f *.ppm

vg: main 
	valgrind --leak-check=full --track-origins=yes ./main

test: vg
	./main > output.txt
	cmp output.txt correct-100.txt

run: main
	./main
