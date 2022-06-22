from ctypes import *
import matplotlib.pyplot as pyplot
import unittest
import random
import math
import time

libmat = CDLL("/home/nathandaly/sdmm-22/initial_materials/mmult.so")

C_INT32_MAX = pow(2, 31) - 1
MAX_SIZE = 1024
MAX_VALUE = 1024

class TestMatrixFuncs(unittest.TestCase):

    ''' Test the C matrix multiplication function against several examples. '''\

    def test_add0(self):

        mat1 = [[2, 1, 4],
                [0, 1, 1]]
        
        mat2 = [[6, 3, -1, 0],
                [1, 1, 0, 4],
                [-2, 5, 0, 2]]

        self.assertEqual(callAdd(mat1, mat2), None)

    def test_add1(self):

        mat1 = [[4, 8],
                [3, 7]]

        mat2 = [[2, 0],
                [6, 1]]

        mat3 = [[6, 8],
                [9, 8]]

        self.assertEqual(callAdd(mat1, mat2), mat3)
    
    def test_mult1(self):
        
        mat1 = [[2, 1, 4],
                [0, 1, 1]]
        
        mat2 = [[6, 3, -1, 0],
                [1, 1, 0, 4],
                [-2, 5, 0, 2]]
        
        prod = [[5, 27, -2, 12],
                [-1, 6, 0, 6]]
        
        self.assertEqual(callMultiply(mat1, mat2), prod)
        
    def test_mult2(self):

        mat1 = [[-1, 4],
                [2, 3]]
        
        mat2 = [[9, -3],
                [6, 1]]
        
        prod = [[15, 7],
                [36, -3]]
        
        self.assertEqual(callMultiply(mat1, mat2), prod)

    def test_mult3(self):
        
        mat1 = [[1, 2, 3],
                [4, 5, 6]]
        
        mat2 = [[10, 11],
                [20, 21],
                [30, 31]]
        
        prod = [[140, 146],
                [320, 335]]
        
        self.assertEqual(callMultiply(mat1, mat2), prod)
        
    def test_mult4(self):
        
        mat1 = [[3, 4, 2]]
        
        mat2 = [[13, 9, 7, 15],
                [8, 7, 4, 6],
                [6, 4, 0, 3]]
        
        prod = [[83, 63, 37, 75]]
        
        self.assertEqual(callMultiply(mat1, mat2), prod)
        
    def test_mult5(self):
        
        mat1 = [[1, 2, 3],
                [4, 5, 6]]
        
        mat2 = [[7],
                [8],
                [9]]
        
        prod = [[50],
                [122]]
        
        self.assertEqual(callMultiply(mat1, mat2), prod)

class MATRIX(Structure):

    ''' Clone of the Matrix struct from the C code. '''
    
    _fields_ = [("rows", c_ushort),
                ("cols", c_ushort),
                ("data", POINTER(c_int))]

def pack(pyMat):

    ''' Converts a Python 2D list into a pointer to a C Matrix struct. '''
    
    cMat = MATRIX()
    cMat.rows = len(pyMat)
    cMat.cols = len(pyMat[0])

    DataType = c_int * (cMat.rows * cMat.cols)
    cMat.data = DataType(*[pyMat[i][j] for i in range(cMat.rows) for j in range(cMat.cols)])
    return pointer(cMat)

def unpack(cMat):

    ''' Converts a pointer to a C Matrix struct into a Python 2D list. '''

    if not cMat: # NULL pointer check
        return None
    
    rows = cMat.contents.rows
    cols = cMat.contents.cols
    pyMat = [[0 for j in range(cols)] for i in range(rows)]
    for i in range(rows):
        for j in range(cols):
            pyMat[i][j] = cMat.contents.data[i * cols + j]
    return pyMat

def callMultiply(mat1, mat2):

    ''' Takes two matrices in their Python representation. 
    Converts them to C and calls the C multiply function,
    then returns the product in its Python representation. '''

    return unpack(cMult(pack(mat1), pack(mat2), pointer(MATRIX())))

def callAdd(mat1, mat2):
    
    ''' Takes two matrices in their Python representation. 
    Converts them to C and calls the C add function,
    then returns the sum in its Python representation. '''

    return unpack(cAdd(pack(mat1), pack(mat2), pointer(MATRIX())))

def selfMultiply(mat1, mat2):

    ''' Multiplies two matrices in Python (because Python > C). 
    Assumes matrices are of correct dimensions. '''

    rows = len(mat1)
    diag = len(mat1[0])
    cols = len(mat2[0])
    prod = [[0 for j in range(cols)] for i in range(rows)]
    total = rows * cols
    cells = 0
    print(f"[{' ' * 50}] 0%")
    for i in range(rows):
        for j in range(cols):
            for k in range(diag):
                prod[i][j] += mat1[i][k] * mat2[k][j]
            cells += 1
            print(f"\u001b[1A[{'=' * (50 * cells // total):<50}] {100 * cells // total}%")
    return prod

def generateMatrix(rows, cols):

    ''' Generate a random matrix with the given number of rows and columns. '''

    assert 0 <= rows <= MAX_SIZE and 0 <= cols <= MAX_SIZE, "Too big!"

    # Bound the value of each cell in the matrices to prevent integer overflow
    limit = math.floor(math.sqrt(C_INT32_MAX / max(rows, cols)))
    if limit > MAX_VALUE:
        limit = MAX_VALUE
    
    return [[random.randint(-limit, limit) for j in range(cols)] for i in range(rows)]
                            
def testRandomMatrices():

    '''
    libmat.setup_func(0, b"main/multiply")
    libmat.setup_func(1, b"element")
    libmat.setup_func(2, b"decompose")
    libmat.setup_func(3, b"compose")
    libmat.setup_func(4, b"compute_m")
    libmat.setup_func(5, b"compute_c")
    libmat.setup_func(6, b"add")
    libmat.setup_func(7, b"subtract")
    libmat.setup_func(8, b"naive_multiply")
    libmat.setup_func(9, b"strassen_multiply")
    '''
    
    #rows = random.randint(1, MAX_SIZE)
    #cols = random.randint(1, MAX_SIZE)
    #diag = random.randint(1, MAX_SIZE)
    rows = cols = diag = 1 << random.randint(1, 10)

    print(f"Generating matrices... ({rows} x {diag}) and ({diag} x {cols})")
    
    time0 = time.time()
    mat1 = generateMatrix(rows, diag)
    mat2 = generateMatrix(diag, cols)
    time1 = time.time()

    print(f"      DONE {time1 - time0} seconds")
    print("Precomputing result...")
    
    time0 = time.time()
    pyProd = selfMultiply(mat1, mat2)
    #pyProd = callMultiply(mat1, mat2, rows + cols + diag)
    time1 = time.time()

    print(f"      DONE {time1 - time0} seconds")
    print("Multiplying matrices...")

    dataX = []
    dataY = []
    dataXY = {}

    markers = ["r+", "ro", "go", "bo"]
    for recursion in range(4):
        libmat.set_parallel_recursion(recursion)
        dataX = []
        dataY = []
        for threshold in [16 << i for i in range(8)]:
            #libmat.reset_stats()
            libmat.set_strassen_threshold(threshold);
            time0 = time.time()
            cProd = callMultiply(mat1, mat2)
            time1 = time.time()
            dataX.append(f"{threshold}")
            dataY.append(time1 - time0)
            dataXY[threshold] = time1 - time0
            result = "PASS" if cProd == pyProd else "FAIL"
            print(f" P{recursion} T{threshold:<4} {result} {time1 - time0} seconds")
        #print("***")
        #for funcID in range(10):
        #    libmat.print_stats(funcID)
        #print("***")
        #print()
    
        print()
        pyplot.plot(dataX, dataY, label=f"{recursion}-layer parallelism")
        
    pyplot.xlabel("Minimum size for Strassen")
    pyplot.ylabel("Runtime (s)")
    pyplot.title(f"Multiplying two {rows} x {cols} matrices")
    pyplot.legend()
    pyplot.show()

# Change return type of the C multiply function!
cMult = libmat.multiply
cAdd = libmat.add
libmat.multiply.restype = POINTER(MATRIX)
libmat.add.restype = POINTER(MATRIX)

if __name__ == '__main__':

    #unittest.main()
    libmat.create_pthread_tools()
    for i in range(int(input("Number of tests: "))):
        testRandomMatrices()
    libmat.destroy_pthread_tools()
