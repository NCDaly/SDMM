CC = gcc
FLAGS = -g
SOURCE = matrix-mult-strassen
FNAME = mmult
TESTER = test_mult.py

all: $(FNAME) $(FNAME).so

$(FNAME): $(SOURCE).c $(SOURCE).h
	$(CC) $(FLAGS) -o $@ $<

$(FNAME).so: $(FNAME).o
	$(CC) $(FLAGS) -o $@ $< -shared

$(FNAME).o: $(SOURCE).c $(SOURCE).h
	$(CC) $(FLAGS) -o $@ $< -c

test: $(FNAME).so
	python3 test_matrix.py

clean:
	rm -f $(FNAME) $(FNAME).o $(FNAME).so
