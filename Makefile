CC   := gcc
COPT := -Wall -g
EXEC := quarto

LDFLAGS := #-lm

SOURCES := $(wildcard *.c) 
OBJECTS := $(SOURCES:.c=.o)

.PHONY: all 

all: COPT  += -g
all: propre $(EXEC)

race: COPT = -Wall -O3 -DNDEBUG -DNPERFT -DNPARALLEL
race: clean $(EXEC)


seq-quarto: COPT = -Wall -O3 -DNDEBUG -DNPERFT -DNPARALLEL
seq-quarto: EXEC = quarto_seq
seq-quarto: clean $(EXEC)


# compile quarto in parallel for tests 
par-quarto: COPT = -Wall -O3 -DNDEBUG -DNPERFT -fopenmp
par-quarto: EXEC = quarto_par
par-quarto: clean $(EXEC)

# compile quarto in parallel for tests 
par-debug-quarto: COPT = -Wall -g -DNPERFT -fopenmp
par-debug-quarto: clean $(EXEC)



para-perft: COPT = -Wall -O3 -DNDEBUG -DNGO
para-perft: clean $(EXEC)

perft: COPT = -Wall -O3 -DNGO -DNPARALLEL -DNDEBUG
perft: propre $(EXEC)

profiler: COPT = -pg -O2 -DNGO -DNPARALLEL
profiler: clean $(EXEC)
profiler: 
	./$(EXEC)
	gprof $(EXEC) | ./gprof2dot.py -n0 -e0 | dot -Tpng -o $(EXEC)-prof.png


$(EXEC): $(OBJECTS) 
	$(CC) $(COPT) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(COPT) -c $< -o $@ $(LDFLAGS)


## Clean workspace
mrproper: 
	rm -f *.o

propre: 
	clear	

clean: mrproper propre
	rm -f $(EXEC) *.out