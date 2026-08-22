CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDLIBS = -lmpfr -lgmp -lm

PROGRAM = legendre_stability
SOURCE = legendre_stability.c
DATA = results.dat
PLOTS = relative_error.png absolute_error.png

.PHONY: all run plot clean

all: plot

$(PROGRAM): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) $(LDLIBS) -o $@

run: $(PROGRAM)
	./$(PROGRAM) > $(DATA)

plot: run plot_results.py
	python3 plot_results.py $(DATA)

clean:
	$(RM) $(PROGRAM) $(DATA) $(PLOTS)
