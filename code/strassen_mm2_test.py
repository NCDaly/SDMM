import ctypes
from ctypes import c_int
from numpy.ctypeslib import ndpointer
import numpy as np

so_file = "/home/bailey/Documents/Summer22/sdmm-22/code/strassen_parallel2.so"

wt = 32
ht = 32
  
m = np.random.randint(2, size=(wt*ht))
n = np.random.randint(2, size=(wt*ht))


#m = [0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1]

#n = [1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0]


print("M ARRAY: ")
print(m)
print("N ARRAY: ")
print(n)

# Using above first method to create a
# 2D array
#presult = [[0]*wt_n for c in range(ht_m)]
#parray_m = [[0]*wt_m for c in range(ht_m)]
#parray_n = [[0]*wt_n for c in range(ht_n)]

#hold = 0
#for l in range(ht_m):
#    for h in range(wt_m):
#        parray_m[l][h] = m[hold]
#        hold += 1

#hold2 = 0
#for y in range(ht_n):
#    for z in range(wt_n):
#        parray_n[y][z] = n[hold2]
#        hold2 += 1

#iterate of rows of m
#for i in range(ht_m):
    #iterate cols Y
#    for j in range(wt_n):
        #iterate rows Y
#        for k in range(ht_n):
#            presult[i][j] += parray_m[i][k] * parray_n[k][j]


#print("PYTHON RESULT:")
#print(presult)


trial = ctypes.CDLL(so_file)
trial.strassen_mm.argtypes = (ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int)
trial.strassen_mm.restype = ndpointer(dtype=c_int, shape=(ht,wt))


#m = [2,1,4,0,1,1]
#n = [6,3,-1,0,1,1,0,4,-2,5,0,2]

m_arraytype = ctypes.c_int * len(m)
n_arraytype = ctypes.c_int * len(n)

results = trial.strassen_mm(m_arraytype(*m), n_arraytype(*n), ctypes.c_int(wt), ctypes.c_int(ht))

print("C RESULT:")
print(results)

#assert (presult == results).all()

#print("Done. Python and C equal")

