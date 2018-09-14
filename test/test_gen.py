import numpy as np

from sys import argv

def generate_test_case(f, l, m, n, lo=0, hi=100):
    ''' 
    Simple function for generating test cases 
    for matrix multiplication library in C++
    '''
    # generate matrices
    # add one to high bound to make it inclusive
    a = np.random.randint(lo, hi+1, size=(l, m))
    b = np.random.randint(lo, hi+1, size=(m, n))

    # compute solution
    c = a.dot(b)

    # write matrices to the file
    write_matrix_to_file(f, a)
    write_matrix_to_file(f, b)
    write_matrix_to_file(f, c)
    
def write_matrix_to_file(f, m):
    '''
    Writes the dimensions of the matrix to a row of the given file
    and the values contained in the matrix to the next row of the file
    '''
    rows, cols = m.shape
    f.write('{} {}\n'.format(rows, cols))

    # flatten the matrix and write each value on a single line
    for value in m.flatten():
        f.write('{} '.format(value))

    f.write('\n')

if __name__ == '__main__':
    max_iters = 50
    iters = 0
    
    if len(argv) < 3:
        print('Usage: python3 test_gen.py outfile maxsize [maxval]')
        exit(1)

    outfile = argv[1]
    maxsize = int(argv[2])

    if len(argv) > 3:
        maxval = int(argv[3])

    else:
        maxval = 25

    with open(outfile, 'w') as product_file:
        # write number of cases at beginning of file
        product_file.write('{}\n'.format(max_iters))
    
        while iters < max_iters:
            l = np.random.randint(1, maxsize)
            m = np.random.randint(1, maxsize)
            n = np.random.randint(1, maxsize)
    
            generate_test_case(product_file, l, m, n, hi=maxval)
    
            print('Test case {} created'.format(iters))
            iters += 1

