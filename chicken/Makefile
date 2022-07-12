CC = gcc
CFLAGS = -lm -std=c99
SOURCE = vandermonde_inverter.c mersenne_prime_field.c
DEPEND = sdmm.h mersenne_prime_field.h
FNAME = vdm_inv

$(FNAME): $(SOURCE) $(DEPEND)
	$(CC) $(CFLAGS) $(SOURCE) -o $@

run: $(FNAME)
	./$(FNAME)

clean:
	rm $(FNAME)
