from ctypes import *
import unittest

libmat = CDLL("/home/nathandaly/sdmm-22/initial_materials/matrix.so")

class TestMult(unittest.TestCase):
    
    def test_mult1(self):
        
        mat1 = [[2, 1, 4],
                [0, 1, 1]]
        
        mat2 = [[6, 3, -1, 0],
                [1, 1, 0, 4],
                [-2, 5, 0, 2]]
        
        prod = [[5, 27, -2, 12],
                [-1, 6, 0, 6]]
        
        self.assertEqual(pyMult(mat1, mat2), prod)
        
    def test_mult2(self):

        mat1 = [[-1, 4],
                [2, 3]]
        
        mat2 = [[9, -3],
                [6, 1]]
        
        prod = [[15, 7],
                [36, -3]]
        
        self.assertEqual(pyMult(mat1, mat2), prod)

    def test_mult3(self):
        
        mat1 = [[1, 2, 3],
                [4, 5, 6]]
        
        mat2 = [[10, 11],
                [20, 21],
                [30, 31]]
        
        prod = [[140, 146],
                [320, 335]]
        
        self.assertEqual(pyMult(mat1, mat2), prod)
        
    def test_mult4(self):
        
        mat1 = [[3, 4, 2]]
        
        mat2 = [[13, 9, 7, 15],
                [8, 7, 4, 6],
                [6, 4, 0, 3]]
        
        prod = [[83, 63, 37, 75]]
        
        self.assertEqual(pyMult(mat1, mat2), prod)
        
    def test_mult5(self):
        
        mat1 = [[1, 2, 3],
                [4, 5, 6]]
        
        mat2 = [[7],
                [8],
                [9]]
        
        prod = [[50],
                [122]]
        
        self.assertEqual(pyMult(mat1, mat2), prod)

class MATRIX(Structure):
    
    _fields_ = [("rows", c_int),
                ("cols", c_int),
                ("data", POINTER(c_int))]

def pack(pyMat):

    cMat = MATRIX()
    cMat.rows = len(pyMat)
    cMat.cols = len(pyMat[0])
    DataType = c_int * (cMat.rows * cMat.cols)
    cMat.data = DataType(*[pyMat[i][j] for i in range(cMat.rows) for j in range(cMat.cols)])
    return pointer(cMat)

def unpack(cMat):

    rows = cMat.contents.rows
    cols = cMat.contents.cols
    pyMat = [[0 for j in range(cols)] for i in range(rows)]
    for i in range(rows):
        for j in range(cols):
            pyMat[i][j] = cMat.contents.data[i * cols + j]
    return pyMat

def pyMult(mat1, mat2):

    return unpack(cMult(pack(mat1), pack(mat2)))

cMult = libmat.multiply
cMult.restype = POINTER(MATRIX)

if __name__ == '__main__':

    unittest.main()
