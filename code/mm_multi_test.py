import ctypes
from ctypes import c_int
from numpy.ctypeslib import ndpointer
import numpy as np

so_file = "/home/bailey/Documents/Summer22/sdmm-22/initial_materials/mm_multi.so"

wt_m = 4
ht_m = 4
wt_n = 1
ht_n = 4  
  
m = np.random.randint(10, size=(wt_m*ht_m))
n = np.random.randint(10, size=(wt_n*ht_n))

print("M ARRAY: ")
print(m)
print("N ARRAY: ")
print(n)

# Using above first method to create a
# 2D array
presult = [[0]*wt_n for c in range(ht_m)]
parray_m = [[0]*wt_m for c in range(ht_m)]
parray_n = [[0]*wt_n for c in range(ht_n)]

hold = 0
for l in range(ht_m):
    for h in range(wt_m):
        parray_m[l][h] = m[hold]
        hold += 1

hold2 = 0
for y in range(ht_n):
    for z in range(wt_n):
        parray_n[y][z] = n[hold2]
        hold2 += 1

#iterate of rows of m
for i in range(ht_m):
    #iterate cols Y
    for j in range(wt_n):
        #iterate rows Y
        for k in range(ht_n):
            presult[i][j] += parray_m[i][k] * parray_n[k][j]


print("PYTHON RESULT:")
print(presult)


trial = ctypes.CDLL(so_file)
trial.mm_multi.argtypes = (ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int)
trial.mm_multi.restype = ndpointer(dtype=c_int, shape=(ht_m,wt_n))


#m = [2,1,4,0,1,1]
#n = [6,3,-1,0,1,1,0,4,-2,5,0,2]

m_arraytype = ctypes.c_int * len(m)
n_arraytype = ctypes.c_int * len(n)

results = trial.mm_multi(m_arraytype(*m), n_arraytype(*n), ctypes.c_int(wt_m), ctypes.c_int(ht_m), ctypes.c_int(wt_n), ctypes.c_int(ht_n))

print("C RESULT:")
print(results)

assert (presult == results).all()

print("Done. Python and C equal")

