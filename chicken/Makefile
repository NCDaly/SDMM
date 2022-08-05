# SDMM 07.19.2022

CXX=gcc
CXXFLAGS=-lm -O3

OBJECTS=interpolate_matrix.o vandermonde_inverter.o sdmm.o mersenne_prime_field.o

all: single_core single_core_strassen sdmm_final_boring sdmm_transpose

single_core: single_core.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) single_core.o $(OBJECTS) -o single_core

single_core_strassen: single_core_strassen.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) single_core_strassen.o $(OBJECTS) -o single_core_strassen

sdmm_final_boring: sdmm_final_boring.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) sdmm_final_boring.o $(OBJECTS) -o sdmm_final_boring

sdmm_transpose: sdmm_transpose.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) sdmm_transpose.o $(OBJECTS) -o sdmm_transpose

sdmm_final_boring.o: sdmm_final_boring.c sdmm.h mersenne_prime_field.h interpolate_matrix.h vandermonde_inverter.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

sdmm_transpose.o: sdmm_transpose.c sdmm.h mersenne_prime_field.h interpolate_matrix.h vandermonde_inverter.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

single_core.o: single_core.c sdmm.h mersenne_prime_field.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

single_core_strassen.o: single_core_strassen.c single_core_strassen.h sdmm.h mersenne_prime_field.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

interpolate_matrix.o: interpolate_matrix.c interpolate_matrix.h sdmm.h mersenne_prime_field.h vandermonde_inverter.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

vandermonde_inverter.o: vandermonde_inverter.c vandermonde_inverter.h sdmm.h mersenne_prime_field.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

sdmm.o: sdmm.c sdmm.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

mersenne_prime_field.o: mersenne_prime_field.c mersenne_prime_field.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

.PHONY: clean
clean:
	-rm -f *.o *~

.PHONY: shmemory
shmemory:
	export SMA_SYMMETRIC_SIZE=2147483648
