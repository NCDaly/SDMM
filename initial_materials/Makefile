CC = gcc
FLAGS = -g -fPIC -pthread
SOURCE = matrix-mult-strassen
STATS = function-stats
FNAME = mmult
TESTER = test_mult.py

all: $(FNAME) $(FNAME).so

$(FNAME): $(FNAME).o $(STATS).o
	$(CC) $(FLAGS) -o $@ $^

$(FNAME).so: $(FNAME).o $(STATS).o
	$(CC) $(FLAGS) -o $@ $^ -shared

$(FNAME).o: $(SOURCE).c $(SOURCE).h
	$(CC) $(FLAGS) -o $@ $< -c

$(STATS).o: $(STATS).c $(STATS).h
	$(CC) $(FLAGS) -o $@ $< -c

test: $(FNAME).so
	python3 test_matrix.py

clean:
	rm -f $(FNAME) $(FNAME).o $(FNAME).so $(STATS).o
