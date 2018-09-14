// Written by Ty Danet

#include <algorithm>
#include <exception>
#include <iostream>
#include <thread>

/**
@brief The matrix class. The crux of any linear algebra library.

Stores values with pointers rather than vectors for performance gains.
*/
class Matrix {
private:
	int rows, cols;
	double **matrix;
	static void multLogic(Matrix*, Matrix*, Matrix*, int, int);
	void swap(Matrix);
public:
	Matrix();
	Matrix(int, int);
	Matrix(const Matrix&);
	Matrix& operator=(const Matrix&);
	~Matrix();

	int getRows();
	int getCols();

	double **linkMatrix();
	double *linkRow(int);

	double getElem(int, int);
	void setElem(int, int, double);

	void display();
	bool equals(Matrix);

	Matrix transpose();
	Matrix multiply(Matrix*);
	Matrix threadedMultiply(Matrix*, int);
};

// publicly defined functions

/**
@brief Default constructor.

Sets rows and columns to zero and matrix to nullptr.
*/
Matrix::Matrix() {
	rows = 0;
	cols = 0;
	matrix = nullptr;
}

/**
@brief Constructor.

Sets the row and column counts
and allocates necessary space.

@param r The number of rows the matrix will have.
@param c The number of columns the matrix will have.
*/
Matrix::Matrix(int r, int c) {
	if (r <= 0 || c <= 0) {
		throw std::invalid_argument("Matrix dimensions must be positive.");
	}
	rows = r;
	cols = c;

	// allocate space for rows
	matrix = new double*[rows];

	for (int i = 0; i < rows; i++) {
		// allocate space for columns
		matrix[i] = new double[cols];
	}
}

/**
@brief Copy Constructor.

@param other The matrix to be copied.
*/
Matrix::Matrix(const Matrix& other) {
	// set dimensions to that of the other matrix
	rows = other.rows;
	cols = other.cols;

	// allocate space for rows
	matrix = new double*[rows];

	for (int i = 0; i < rows; i++) {
		// allocate space for columns
		matrix[i] = new double[cols];
	}
	// copy each element of the other matrix into copy
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix[i][j] = other.matrix[i][j];
		}
	}
}

/**
@brief Assignment operator.

@param other Matrix being assigned to this matrix.
@return Matrix with new values.
*/
Matrix& Matrix::operator=(const Matrix& other) {
	// copy the other matrix with copy constructor
	Matrix tmp(other);
	// swap values of this with tmp
	swap(tmp);
	// return swapped matrix
	return *this;
}

/**
@brief Destructor.
*/
Matrix::~Matrix() {
	for (int i = 0; i < rows; i++) {
		// deallocate columns
		delete[] matrix[i];
	}
	// deallocate rows
	delete[] matrix;
}

/**
@brief Row getter method.

@return The row count of the matrix.
*/
int Matrix::getRows() { return rows; }

/**
@brief Column getter method.

@return The column count of the matrix.
*/
int Matrix::getCols() { return cols; }

/**
@brief Links matrix to variable.

Returns pointer to the matrix instance variable.
Potentially unsafe since the 2D matrix array may be destructively altered.

@return Pointer to the 2D array containing the values.
*/
double **Matrix::linkMatrix() { return matrix; }

/**
@brief Links row to variable.

Returns pointer to the ith row of the matrix instance variable.
Potentially unsafe since the row array may be destructively altered.

@return Pointer to the array containing the values in the ith row of the matrix.
*/
double *Matrix::linkRow(int i) {
	if (i < 0 || i > rows) {
		throw std::invalid_argument("Row index out of bounds.");
	}
	return matrix[i];
}

/**
@brief Element getter method.

@param i The ith row of the matrix.
@param j The jth row of the matrix.
@return The ijth element of the matrix.
*/
double Matrix::getElem(int i, int j) {
	if(i < 0 || j < 0 || i > rows || j > cols) {
		throw std::invalid_argument("Coordinates out of bounds.");
	}
	return matrix[i][j];
}

/**
@brief Element setter method.

@param i The ith row of the matrix.
@param j The jth row of the matrix.
@param val The value to be placed in the ijth element of the matrix.
*/
void Matrix::setElem(int i, int j, double val) {
	if(i < 0 || j < 0 || i > rows || j > cols) {
		throw std::invalid_argument("Coordinates out of bounds.");
	}
	matrix[i][j] = val;
}

/**
@brief Prints the elements of the matrix in a more readable format.
*/
void Matrix::display() {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			// print the row
			std::cout << matrix[i][j] << " ";
		}
		// newline for the new row
		std::cout << std::endl;
	}
	// extra newline for when matrices are displayed in succession
	std::cout << std::endl;
}

/**
@brief Checks if two matrices are equal.

@Return True if matrices are equal, false otherwise.
*/
bool Matrix::equals(Matrix other) {
	// no need for further computation if dimensions are not identical
	if (rows != other.rows || cols != other.cols) {
		return false;
	}
	// assume the matrices are equal until proven otherwise
	bool truth = true;

	// extra loop condition to account for inner break statement
	for (int i = 0; (i < rows) && truth; i++) {
		for (int j = 0; j < cols; j++) {
			// matrix inequality proven, no need to continue
			if (matrix[i][j] != other.matrix[i][j]) {
				truth = false;
				break;
			}
		}
	}
	return truth;
}

/**
@brief Simple matrix transposition.

@return A new Matrix object that is the transpose of the original matrix.
*/
Matrix Matrix::transpose() {
	// create Matrix object with rows and columns swapped
	Matrix transposition(cols, rows);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			// place elements in opposite coordinates
			transposition.matrix[j][i] = matrix[i][j];
		}
	}
	return transposition;
}

/**
@brief Matrix multiplication.

Single threaded matrix multiplication.
Necessary for when the computations are not 
worth the overhead of creating threads.

@param other Pointer to a matrix to multiply with the current object.
@return A new Matrix object that is the product of the two matrices.
*/
Matrix Matrix::multiply(Matrix *other) {
	if (cols != other->rows) {
		throw std::invalid_argument("Matrix dimensions not compatible for multiplication");
	}
	// initialize product matrix with appropriate size
	Matrix product(rows, other->cols);
	// compute transposition of m
	Matrix otherTranspose = other->transpose();

	// call multLogic with offset 0 and skip 1 
	// to perform multiplation on every row
	multLogic(this, &otherTranspose, &product, 0, 1);

	return product;
}

/**
@brief Matrix multiplication with multithreading.

@param other Pointer to a matrix to multiply with the current object.
@param nThreads The number of threads to use in the computation.
@return A new Matrix object that is the product of the two matrices.
*/
Matrix Matrix::threadedMultiply(Matrix *other, int nThreads) {
	if (cols != other->rows) {
		throw std::invalid_argument("Matrix dimensions not compatible for multiplication");
	}
	// initialize product matrix with appropriate size
	Matrix product(rows, other->cols);
	// compute transposition of m
	Matrix otherTranspose = other->transpose();
	// set up thread array
	std::thread threads[nThreads];

	for (int i = 0; i < nThreads; i++) {
		// call multLogic with offset i and skip equal to number of threads
		// so threads alternate between rows
		threads[i] = std::thread(multLogic, this, &otherTranspose, &product, i, nThreads);
	}
	for (int i = 0; i < nThreads; i++) {
		// wait for threads to complete
		threads[i].join();
	}
	return product;
}

// privately defined functions

/**
@brief Matrix multiplication logic.

Matrix multiplication algorithm used by 
multiply and threadedMultiply functions.

It's an implementation of naive matrix multiplication 
with a few optimizations. Uses transposition to reduce 
cache misses and tries to cut down on access times by 
not using doubly-indexed arrays.

@param a Pointer to left-hand matrix of the product
@param bT Pointer to transposition of right-hand matrix of the product
@param product Pointer to product of a and b
@param offset Tells the algorithm which row to start from
@param skip Tells the algorithm how many rows to skip each iteration
*/
void Matrix::multLogic(Matrix *a, Matrix *bT, Matrix *product, int offset, int skip) {
	for (int i = offset; i < a->getRows(); i+=skip) {
		// point to ith rows of a and product matrices 
		double *ithRowA = a->linkRow(i);
		double *ithRowProduct = product->linkRow(i);

		for (int j = 0; j < bT->getRows(); j++) {
			// point to the jth row of bT
			double *jthRowBT = bT->linkRow(j);
			// initialize sum to zero
			ithRowProduct[j] = 0;

			for (int k = 0; k < a->getCols(); k++) {
				// equivalent to C(i, j) += A(i, k) * B^T(j, k)
				// or also to C(i, j) += A(i, k) * B(k, j)
				ithRowProduct[j] += ithRowA[k] * jthRowBT[k];
			}
		}
	}
}

/**
@brief Swap function used by assignment operator.

@param other Matrix to swap values with
*/
void Matrix::swap(Matrix other) {
	std::swap(rows, other.rows);
	std::swap(cols, other.cols);
	std::swap(matrix, other.matrix);
}
