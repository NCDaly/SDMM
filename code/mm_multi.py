import ctypes
from ctypes import c_int
from numpy.ctypeslib import ndpointer

so_file = "/home/bailey/Documents/Summer22/sdmm-22/initial_materials/mm_multi.so"

wt_m = 3
ht_m = 2
wt_n = 4
ht_n = 3
 
trial = ctypes.CDLL(so_file)
trial.mm_multi.argtypes = (ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int)
trial.mm_multi.restype = ndpointer(dtype=c_int, shape=(ht_m,wt_n))
 
#print(type(trial))
 
m = [2,1,4,0,1,1]
n = [6,3,-1,0,1,1,0,4,-2,5,0,2]
 
m_arraytype = ctypes.c_int * len(m)
n_arraytype = ctypes.c_int * len(n)
#array_type = ctypes.c_int * 3
results = trial.mm_multi(m_arraytype(*m), n_arraytype(*n), ctypes.c_int(wt_m), ctypes.c_int(ht_m), ctypes.c_int(wt_n), ctypes.c_int(ht_n))

#print("The answer is: ", results[0])
#print("The answer2 is: ", results[1])
#print("the answer3 is: ", results[2])
#print("The answer4 is: ", results[3])
print(results)

#trial = cdll.LoadLibrary(so_file)
#c_sum = lib.c_sum
#c_sum.restype = ndpointer(dtype=c_double,shape=(n,))

#trial.mv_multi(c_void_p(vector.ctypes.data),c_void_p(matrix.ctypes.data),c_int(wt),c_int(    ht))

print("Done")

