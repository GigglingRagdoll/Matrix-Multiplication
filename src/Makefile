output: test.o
	g++ test.o -pthread -o test

test.o: test.cpp
	g++ -c -std=c++11 -O3 test.cpp

clean:
	rm *.o test

