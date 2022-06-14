import ctypes
from ctypes import c_int
from numpy.ctypeslib import ndpointer

so_file = "/home/bailey/Documents/Summer22/sdmm-22/initial_materials/mv_multi.so"

wt = 3
ht = 4

trial = ctypes.CDLL(so_file)
trial.mv_multi.argtypes = (ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int), ctypes.c_int, ctypes.c_int)
trial.mv_multi.restype = ndpointer(dtype=c_int, shape=(ht,))

#print(type(trial))

vector = [1,-1,-1]
matrix = [1,-1,-1,1,-1,1,1,1,-1,-1,-1,-1]

v_arraytype = ctypes.c_int * len(vector)
m_arraytype = ctypes.c_int * len(matrix)
#array_type = ctypes.c_int * 3
results = trial.mv_multi(v_arraytype(*vector), m_arraytype(*matrix), ctypes.c_int(wt), ctypes.c_int(ht), ctypes.c_int(len(matrix)))

#print("The answer is: ", results[0])
#print("The answer2 is: ", results[1])
#print("the answer3 is: ", results[2])
#print("The answer4 is: ", results[3])
print(results)

#trial = cdll.LoadLibrary(so_file)
#c_sum = lib.c_sum
#c_sum.restype = ndpointer(dtype=c_double,shape=(n,))

#trial.mv_multi(c_void_p(vector.ctypes.data),c_void_p(matrix.ctypes.data),c_int(wt),c_int(ht))

print("Done")
