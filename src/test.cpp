#include <chrono>
#include <cstdlib>
#include <fstream>
#include <string>

#include "matrix.hpp"

/**
Reads in matrices from files formatted as such:

m n

a00 a01 ... a0n a10 a11 ... a1n ... am0 am1 ... amn

where m is the number of rows of the matrix,
n is the number of columns of the matrix, and
aij is the ijth element of the matrix.
*/
Matrix readFromFile(std::ifstream& infile) {
	int rows, cols;
	// stores each value as we read them in
	double currElem;

	infile >> rows >> cols;
	std::cout << "Reading in " << rows << "x" << cols << " matrix." << std::endl;

	Matrix m(rows, cols);
	
	for (int i = 0; i < rows * cols; i++) {
		infile >> currElem;
		// place each element in appropriate row and column
		m.linkRow(i / cols)[i % cols] = currElem;
	}
	return m;
}

/**
Quick and dirty test run.
*/
int main(int argc, char **argv) {

	int N; // number of test cases
	int threads; // number of threads to use
	int display; // flag for showing more or less output
	// flag that allows or disallows matrices with incompatible
	// dimensions for matrix multiplaction
	// set by user on the first detection of mismatched matrices
	char mismatch = 0; 

	// determine number of threads to use
	if (argc < 3) {
		std::cout << "Usage: " << argv[0] << " numthreads infile [display]" << std::endl;
		exit(1);

	} 
	if (argc < 4) {
		threads = atoi(argv[1]);
		// 3 or less arguments the program just assumes you want to see less output
		display = 0;

	} else {
		// 4 or more arguments the program just assumes you want to see more output
		display = 1;
	}
	// read from test file
	std::ifstream infile(argv[2]);

	infile >> N;
	std::cout << "Testing on " << N << " cases." << std::endl;

	// start timing from the beginning of the test
	auto start = std::chrono::high_resolution_clock::now();

	for(int i = 0; i < N; i++) {
		std::cout << "TEST CASE " << i << std::endl;
		Matrix a = readFromFile(infile);
		Matrix b = readFromFile(infile);
		Matrix soln = readFromFile(infile);

		Matrix test;

		try {
			// use single or multithreaded function depending on input from earlier
			if (threads > 1) {
				test = a.threadedMultiply(&b, threads);

			} else {
				test = a.multiply(&b);
			}

		} catch (std::invalid_argument& e) {
			std::cout << "Matrix dimensions did not match up in this test case." << std::endl;
			std::cout << "Was that your intent? (y/n) >>> ";


			if (mismatch == 0) {
				std::cin >> mismatch;
			
			}
		 	if (mismatch == 'y' || mismatch == 'Y') {
				std::cout << "PASSED" << std::endl;
				continue;
			
			} else {
				std::cout << "The most likely problem may be the test file used." << std::endl;
				std::cout << "Make sure it is uncorrupted and/or formatted correctly for this test." << std::endl;
				std::cout << "Exiting." << std::endl;
				exit(1);
			}
		}

		if (display) {
			a.display();
			b.display();
			soln.display();
			test.display();
		}
		// check computed matrix and solution matrix for equality
		if (test.equals(soln)) {
			std::cout << "PASSED" << std::endl;

		} else {
			std::cout << "FAILED" << std::endl;
			break;	// stop the test if there are any failures
		}
	}
	// capture ending time
	auto end = std::chrono::high_resolution_clock::now();
	// compute time taken to finish test
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Total time: " << elapsed.count() << std::endl;
	return 0;
}
