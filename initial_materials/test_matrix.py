from ctypes import *

libmat = CDLL("/home/nathandaly/sdmm-22/initial_materials/matrix.so")

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

input_matrix = libmat.input_matrix
input_matrix.restype = POINTER(MATRIX)

multiply = libmat.multiply
multiply.restype = POINTER(MATRIX)

print_matrix = libmat.print_matrix
print_matrix.restype = None

destroy_matrix = libmat.destroy_matrix
destroy_matrix.restype = None

mat1 = pack([[2, 1, 4],
             [0, 1, 1]])
mat2 = pack([[6, 3, -1, 0],
             [1, 1, 0, 4],
             [-2, 5, 0, 2]])
prod = multiply(mat1, mat2)
pyProd = unpack(prod)
for row in pyProd:
    print(*row)
expected = [[5, 27, -2, 12],
            [-1, 6, 0, 6]]
print(pyProd == expected)
