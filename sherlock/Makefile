# SDMM 07.19.2022

CXX=oshcc
CXXFLAGS=-lm -O3

OBJECTS=sdmm_final_boring.o interpolate_matrix.o vandermonde_inverter.o sdmm.o mersenne_prime_field.o

sdmm_final:$(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o sdmm_final

sdmm_final_boring.o:sdmm_final_boring.c sdmm.h mersenne_prime_field.h interpolate_matrix.h vandermonde_inverter.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

interpolate_matrix.o:interpolate_matrix.c interpolate_matrix.h sdmm.h mersenne_prime_field.h vandermonde_inverter.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

vandermonde_inverter.o:vandermonde_inverter.c vandermonde_inverter.h sdmm.h mersenne_prime_field.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

sdmm.o:sdmm.c sdmm.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

mersenne_prime_field.o:mersenne_prime_field.c mersenne_prime_field.h
	$(CXX) $(CXXFLAGS) $< -c -o $@

.PHONY: clean
clean:
	-rm -f *.o *~ a.out

.PHONY: export SMA_SYMMETRIC_SIZE=2147483648
