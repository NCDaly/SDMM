from ctypes import *
import unittest
import random
import math
import time

libmat = CDLL("/home/nathandaly/sdmm-22/initial_materials/mmult.so")

C_INT32_MAX = pow(2, 31) - 1
MAX_SIZE = 1024
MAX_VALUE = 1024

class TestMatrixMult(unittest.TestCase):

    ''' Test the C matrix multiplication function against several examples. '''
    
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

    return unpack(cMult(pack(mat1), pack(mat2)))

def selfMultiply(mat1, mat2):

    ''' Multiplies two matrices in Python (because Python > C). 
    Assumes matrices are of correct dimensions. '''

    rows = len(mat1)
    diag = len(mat1[0])
    cols = len(mat2[0])
    prod = [[0 for j in range(cols)] for i in range(rows)]
    for i in range(rows):
        for j in range(cols):
            for k in range(diag):
                prod[i][j] += mat1[i][k] * mat2[k][j]
    return prod

def generateMatrix(rows, cols):

    ''' Generate a random matrix with the given number of rows and columns. '''

    assert 0 <= rows <= MAX_SIZE and 0 <= cols <= MAX_SIZE, "Too big!"

    # Bound the value of each cell in the matrices to prevent integer overflow
    limit = math.floor(math.sqrt(C_INT32_MAX / max(rows, cols)))
    if limit > MAX_VALUE:
        limit = MAX_VALUE
    
    return [[random.randint(-limit, limit) for j in range(cols)] for i in range(rows)]
    #print(f"Generated matrix: {rows} rows, {cols} cols.")
    #for row in mat:
    #    print(*[f"{elem:4}" for elem in row])
    return mat
                            
def testRandomMatrices():

    rows = random.randint(1, MAX_SIZE)
    cols = random.randint(1, MAX_SIZE)
    diag = random.randint(1, MAX_SIZE)

    print(f"Generating matrices... ({rows} x {diag}) and ({diag} x {cols})")
    
    mat1 = generateMatrix(rows, diag)
    mat2 = generateMatrix(diag, cols)
    pyProd = selfMultiply(mat1, mat2)

    print(f" Multiplying matrices...")

    time0 = time.time()
    cProd = callMultiply(mat1, mat2)
    time1 = time.time()
    
    print(f"  Pass: {cProd == pyProd}")
    print(f"  Time: {time1 - time0} seconds")
    print(f"  Total multiplications: {rows * cols * diag}")
    print(f"  Multiplications / sec: {(rows * cols * diag) // (time1 - time0)}")


# Change return type of the C multiply function!
cMult = libmat.multiply
libmat.multiply.restype = POINTER(MATRIX)

if __name__ == '__main__':

    #unittest.main()
    for i in range(int(input("Number of tests: "))):
        testRandomMatrices()
